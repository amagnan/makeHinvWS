#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>

#include "TSystem.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooArgList.h"
#include "TH1F.h"
#include "RooDataHist.h"
#include "RooFormulaVar.h"
//#include "../../interface/RooParametricHist.h"
#include "../HiggsAnalysis/CombinedLimit/interface/RooParametricHist.h"
//#include "/afs/cern.ch/user/v/vmilosev/CMSSW_10_2_13/src/HiggsAnalysis/CombinedLimit/interface/RooParametricHist.h"
#include "RooAddition.h"

// This script produces workspaces to make the signal and smaller background nominal and uncertainty shapes 

enum PROCESS{
  VBFH   = 0,
  ggH    = 1,
  WH     = 2,
  qqZH   = 3,
  ggZH   = 4,
  ttH    = 5,
  TOP    = 6,
  VV     = 7,
  DY     = 8,
  EWKZll = 9
};

double findmax(TH1F *h){
  
  double maxbv = h->GetBinContent(1);
  for (int b=2;b<=h->GetNbinsX();b++){
	double v = h->GetBinContent(b);
	if (fabs(v)<0.00001) continue;
	if (v > maxbv) maxbv = v;
  }
  return maxbv;

};
double findmin(TH1F *h){
  
  double minbv = h->GetBinContent(1);
  for (int b=2;b<=h->GetNbinsX();b++){
	double v = h->GetBinContent(b);
	if (fabs(v)<0.00001) continue;
	if (v < minbv) minbv = v;
  }
  return minbv;

};
void makePlot(TDirectory *where, std::string name, std::string sys, TH1F *hC, TH1F* hU, TH1F*hD){
	TCanvas *can = new TCanvas((name+"_"+sys).c_str(),"Syst plot",700,660);
	TPad p1("p1","p1",0.0,0.3,1,0.98);
	TPad p2("p2","p2",0.0,0.01,1,0.28);
	can->cd();
	p1.Draw();
	p2.Draw();
	TLegend leg(0.6,0.78,0.89,0.89);
	leg.AddEntry(hC,"Nominal","L");
	leg.AddEntry(hU,(sys+"Up").c_str(),"L");
	leg.AddEntry(hD,(sys+"Down").c_str(),"L");

	hC->SetTitle(name.c_str());
	hC->GetYaxis()->SetTitle("Events");
	hC->GetXaxis()->SetTitle("M_{jj} (GeV)");
	hC->SetLineWidth(2);
	hC->SetLineColor(1);
	p1.cd();
	hC->Draw("hist");

	hU->SetLineWidth(2);
	hD->SetLineWidth(2);
	hU->SetLineColor(2);
	hD->SetLineColor(2);
	hD->SetLineStyle(2);
        hU->Draw("histsame");	
        hD->Draw("histsame");
	leg.Draw();
	p1.SetLogy();

	TH1F *hUr = (TH1F*)hU->Clone();
	TH1F *hDr = (TH1F*)hD->Clone();
	hUr->Divide(hC);
	hDr->Divide(hC);

	p2.cd();
	p2.SetGridy();
	p2.SetGridx();
	hUr->SetTitle("");
	hUr->GetYaxis()->SetTitle("Syst/Nominal");
	hUr->GetYaxis()->SetLabelSize(0.08);
	hUr->GetYaxis()->SetTitleSize(0.08);
	hUr->GetYaxis()->SetTitleOffset(0.6);
	double maxup = findmax(hUr);
	double maxdn = findmax(hDr);
	double minup = findmin(hUr);
	double mindn = findmin(hDr);
	hUr->SetMaximum(1.02*maxup);
	hUr->SetMinimum(0.98*minup);
	if (maxup<maxdn) hUr->SetMaximum(1.02*maxdn);
	if (minup>mindn) hUr->SetMinimum(0.98*mindn);
	hUr->Draw("hist");
	hDr->Draw("histsame");

	// Uncomment this to get plots for each one (in addition to being saved in the ROOT file)
	//can->SaveAs(Form("%s.pdf",can->GetName()));
	where->WriteTObject(can);
	//return can; 
 };

void makeSignalAndMCBackgroundWS(std::string year="2017", std::string cat="MTR"){


  const bool doSamSetup = true;
  const bool doEMSF = false;
  
  const bool is2017 = year=="2017";
  std::string lChannel = "VBF";
  std::string lCategory = cat+"_";
  std::string lYear = year+"_";
  std::string lOutFileName = "signal_mc_bkgs_ws_"+lCategory+lYear+lChannel+".root";
  
  //define the variable that is fitted
  RooRealVar lVarFit(("mjj_"+cat+"_"+year).c_str(),"M_{jj} (GeV)",200,5000);
  std::string lVarLabel = "Mjj";
  
  TFile *fOut = new TFile(lOutFileName.c_str(),"RECREATE");
  RooWorkspace wspace("wspace_signal","wspace_signal");
  RooArgList vars(lVarFit);
  
  //finput->cd(lRegions.c_str());
  
  TFile *finputJES = TFile::Open("../vbf_shape_jes_uncs_smooth.root");
  
  const unsigned nP = 10;
  //!! Mind that order is the same as in PROCESS enum above !!
  std::string lProcs[nP]    = {"VBFHtoInv","GluGluHtoInv","WH","qqZH","ggZH","ttH","TOP","VV","DY","EWKZll"};
  std::string lJESLabel[nP] = {"VBF","ZJetsToNuNu","ZJetsToNuNu","ZJetsToNuNu","ZJetsToNuNu","ZJetsToNuNu","ZJetsToNuNu","ZJetsToNuNu","ZJetsToNuNu","EWKZ2Jets_ZToNuNu"};  // for now, use the Z->vv sample calculation for ggH, WH, qqZH, ggZH, ttH, VV and Top
  
  const unsigned nN = doEMSF ? 17 : 16;
  std::string lSysts[17] = {"bjet_veto","pileup","tau_veto",
			    "eventVetoVEleIdIso", "eventVetoVEleReco", 
			    "eventVetoLMuId","eventVetoLMuIso",
			    "eventSelTEleIdIso","eventSelTEleReco",
			    "eventSelVEleIdIso","eventSelVEleReco", 
			    "eventSelTMuId","eventSelTMuIso",
			    "eventSelLMuId","eventSelLMuIso",
			    "prefiring","jetemSF"
  };

  //hardcode bool for skipping syst for given proc
  bool skipSyst[nP][nN];
  for (unsigned iP(0); iP<nP; ++iP){
    for (unsigned iN(0); iN < nN; ++iN){
      skipSyst[iP][iN] = false;
    }
  }
  
  skipSyst[PROCESS::TOP][nN-1] = true;
  skipSyst[PROCESS::VV][nN-1] = true;
  skipSyst[PROCESS::DY][nN-1] = true;
  skipSyst[PROCESS::EWKZll][nN-1] = true;


  std::string lSystsCMS[17] = {
    "CMS_eff_bveto","CMS_pileup","CMS_eff_tauveto",
    "CMS_eff_eVeto_idiso_veto","CMS_eff_eVeto_reco_veto",
    "CMS_eff_muLoose_id_veto","CMS_eff_muLoose_iso_veto",
    "CMS_eff_eTight_idiso","CMS_eff_eTight_reco",
    "CMS_eff_eVeto_idiso","CMS_eff_eVeto_reco",
    "CMS_eff_muTight_id","CMS_eff_muTight_iso",
    "CMS_eff_muLoose_id","CMS_eff_muLoose_iso",
    "CMS_L1prefire","CMS_eff_jetNEMF"
  };
  
  const bool corrCat[17] = {1,1,1,
			    1,1,
			    1,1,
			    1,1,
			    1,1,
			    1,1,
			    1,1,
			    1,1
  };
  const bool corrYear[17] = {1,1,0,
			     0,1,
			     1,1,
			     0,1,
			     0,1,
			     1,1,
			     1,1,
			     0,0
  };
  
  for (unsigned iN(0); iN < nN; ++iN){
    if (!corrCat[iN]) lSystsCMS[iN] +="_"+cat;
    if (!corrYear[iN]) lSystsCMS[iN] += "_"+year;
  }
  
  const unsigned nR = 5;
  std::string lRegions[nR] = {"SR","Zee","Zmumu","Wenu","Wmunu"};
  
  const unsigned nJ = 11;
  std::string lJes[nJ] = {
    "jesAbsolute"
    , Form("jesAbsolute_%s",year.c_str())
    , "jesBBEC1"
    , Form("jesBBEC1_%s",year.c_str())
    , "jesEC2"
    , Form("jesEC2_%s",year.c_str())
    , "jesFlavorQCD"
    , "jesHF"
    , Form("jesHF_%s",year.c_str())
    , "jesRelativeBal"
    , Form("jesRelativeSample_%s",year.c_str())
  };
  
  
  TDirectory *outPlots = fOut->mkdir("Plots");
  
  for (unsigned iR(0); iR<nR; ++iR){
    //std::string lRegions = "SR";
    std::string lInFileName = "out_VBF_ana_"+lRegions[iR]+"_"+year+"_v"+cat+"_"+year+"_200109/VBF_shapes.root";
    TFile *finput = TFile::Open(lInFileName.c_str());
    if (!finput) return ;
    std::string channel=""; 
    if ( doSamSetup && iR>0 ) channel="VBF";
    for (unsigned iP(0); iP<nP; ++iP){
      if (iR>0 and iP<PROCESS::TOP) continue;
      std::cout << " central histogram -- " << Form("%s%s/%s",lRegions[iR].c_str(),channel.c_str(),lProcs[iP].c_str()) << std::endl; 
      TH1F* Thist = (TH1F*)finput->Get(Form("%s%s/%s",lRegions[iR].c_str(),channel.c_str(),lProcs[iP].c_str()));
      
      if (!Thist) {
	std::cout << " Histo " << Form("%s%s/%s",lRegions[iR].c_str(),channel.c_str(),lProcs[iP].c_str()) << " not found." << std::endl;
	//continue;
	return ;
      }
      RooDataHist *hist = new RooDataHist((lProcs[iP]+"_hist_"+lRegions[iR]).c_str(),"Process",vars,Thist);
      wspace.import(*hist);
      
      for (unsigned iS(0); iS < nN; ++iS){
	if (skipSyst[iP][iS]) {
	  std::cout << " -- Skip systematic " << lSysts[iS] << " for process " << lProcs[iP] << std::endl;
	  continue;
	}
	std::cout << " up/down histograms -- " << lSysts[iS].c_str() << " " << lSystsCMS[iS].c_str() << " , for " << Form("%s%s/%s",lRegions[iR].c_str(),channel.c_str(),lProcs[iP].c_str()) << std::endl; 
	
	TH1F* ThistSU = (TH1F*)finput->Get(Form("%s%s/%s_%sUp",lRegions[iR].c_str(),channel.c_str(),lProcs[iP].c_str(),lSysts[iS].c_str()));
	if (!ThistSU) {
	  std::cout << " Histo " << Form("%s%s/%s_%sUp",lRegions[iR].c_str(),channel.c_str(),lProcs[iP].c_str(),lSysts[iS].c_str()) << " not found" << std::endl;
	  continue;
	} 
	RooDataHist *histSU = new RooDataHist((lProcs[iP]+"_hist_"+lRegions[iR]+"_"+lSystsCMS[iS]+"Up").c_str(),"proces",vars,ThistSU);
	wspace.import(*histSU);	    
	
	TH1F* ThistSD = (TH1F*)finput->Get(Form("%s%s/%s_%sDown",lRegions[iR].c_str(),channel.c_str(),lProcs[iP].c_str(),lSysts[iS].c_str()));
	RooDataHist *histSD = new RooDataHist((lProcs[iP]+"_hist_"+lRegions[iR]+"_"+lSystsCMS[iS]+"Down").c_str(),"proces",vars,ThistSD);
	wspace.import(*histSD);	     
	
	makePlot(outPlots, (lProcs[iP]+"_hist_"+lRegions[iR]),lSysts[iS],Thist,ThistSU,ThistSD);
      }
      
      std::cout << " Should all be good? " << Thist->GetName() << std::endl;
      // now create the variations due to the different sources of JES
      std::cout << finputJES->GetName() <<std::endl;
      for (unsigned iJ(0); iJ < nJ; ++iJ){
	
	
	TH1F *hSUp   = (TH1F*)finputJES->Get(Form("%s%s_%sUp",lJESLabel[iP].c_str(),year.c_str(),lJes[iJ].c_str()));
	TH1F *hSDown = (TH1F*)finputJES->Get(Form("%s%s_%sDown",lJESLabel[iP].c_str(),year.c_str(),lJes[iJ].c_str()));
	std::cout << " Getting JES files for " << Thist->GetName() << std::endl;
	
	TH1F *hSUpnew = (TH1F*)Thist->Clone(); hSUpnew->SetName(Form("%s%s_%sUp",lRegions[iR].c_str(),lProcs[iP].c_str(),lJes[iJ].c_str()));
	TH1F *hSDownnew = (TH1F*)Thist->Clone(); hSDownnew->SetName(Form("%s%s_%sDown",lRegions[iR].c_str(),lProcs[iP].c_str(),lJes[iJ].c_str()));
	std::cout << " Filling Up/Down JES for " << Thist->GetName() << std::endl;
	for (int b=1; b <= Thist->GetNbinsX() ; b++){
	  double xv = hSUpnew->GetBinCenter(b);
	  double yv = hSUpnew->GetBinContent(b);
	  if ( xv > hSUp->GetBinLowEdge(hSUp->GetNbinsX()) ) xv = hSUp->GetBinLowEdge(hSUp->GetNbinsX())+0.5*hSUp->GetBinWidth(hSUp->GetNbinsX());
	  
	  hSUpnew->SetBinContent(b,yv*hSUp->GetBinContent(hSUp->FindBin(xv)));
	  hSDownnew->SetBinContent(b,yv*hSDown->GetBinContent(hSDown->FindBin(xv)));
	}
	
	RooDataHist *histSU = new RooDataHist((lProcs[iP]+"_hist_"+lRegions[iR]+"_CMS_scale_j_"+lJes[iJ]+"Up").c_str(),"proces",vars,hSUpnew);
	RooDataHist *histSD = new RooDataHist((lProcs[iP]+"_hist_"+lRegions[iR]+"_CMS_scale_j_"+lJes[iJ]+"Down").c_str(),"proces",vars,hSDownnew);
	wspace.import(*histSU);
	wspace.import(*histSD);
	
	makePlot(outPlots, (lProcs[iP]+"_hist_"+lRegions[iR]),lJes[iJ],Thist,hSUpnew,hSDownnew);
	
      }
      
    }
  }
  
  
  fOut->WriteTObject(&wspace);
  
};

