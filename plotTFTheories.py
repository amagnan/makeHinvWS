import ROOT 
import plotRatios 
import sys

ROOT.gStyle.SetOptStat(0)
ROOT.gROOT.SetBatch(1)


BASE_DIRECTORY="fast_datacard_input_200127"

"""
tf.ZProc = "mumu"
tf.WProc = "munu"
tf.ZR = "MUMU"
tf.WR = "MUNU"
"""

# inputs are cat ZProc, WProc, ZR, WR, ytitle, ymin, ymax, outname
tf = plotRatios.TFValidator("%s/test_df_%s2020v1/all_percategory.root"%(BASE_DIRECTORY,sys.argv[1]),"%s/test_df_%s2020v1/fitDiagnostics.root"%(BASE_DIRECTORY,sys.argv[1]))

tf.cat   = sys.argv[1]
tf.ZProc = sys.argv[2]
tf.WProc = sys.argv[3]
tf.ZR = sys.argv[4]
tf.WR = sys.argv[5]

ytitle = sys.argv[6]
ymin = sys.argv[7]
ymax = sys.argv[8]
out  = sys.argv[9]

lstr = sys.argv[10]#
clab = sys.argv[11]#

fdummy = ROOT.TFile.Open("%s/test_df_%s2020v1/fitDiagnostics.root"%(BASE_DIRECTORY,sys.argv[1]))
hdummy = fdummy.Get("shapes_prefit/SR/VBFHtoInv")

data = hdummy.Clone(); data.SetName("data")
rata  = hdummy.Clone(); rata.SetName("ratio")
ratae = hdummy.Clone(); ratae.SetName("ratio")
ratae_nostat = hdummy.Clone(); ratae_nostat.SetName("ratio_nostat")
ratae_noexp = hdummy.Clone(); ratae_noexp.SetName("ratio_noexp")

nbins = hdummy.GetNbinsX() 

for b in range(1,nbins+1):
  de = tf.calcRdata(b)
  data.SetBinContent(b,de[0])
  data.SetBinError(b,de[1])

  rth = tf.calcR(b)
  print rth, de[0]
  rata.SetBinContent(b,rth)
  rata.SetBinError(b,0)
  ratae.SetBinContent(b,rth)
  ratae.SetBinError(b,tf.returnRMS(b,True,True))

  ratae_noexp.SetBinContent(b,rth)
  ratae_noexp.SetBinError(b,tf.returnRMS(b,True,False))

  ratae_nostat.SetBinContent(b,rth)
  ratae_nostat.SetBinError(b,tf.returnRMS(b,False,False))


data.SetMarkerStyle(20)
data.SetMarkerSize(1.2)
data.SetLineWidth(2)
data.SetLineColor(1)

ratae_nostat.SetLineColor(0)
ratae_nostat.SetLineWidth(3)
ratae_noexp.SetLineColor(0)
ratae_noexp.SetLineWidth(3)
ratae.SetLineColor(0)
ratae.SetLineWidth(3)
rata.SetLineColor(4)
rata.SetLineWidth(3)

c = ROOT.TCanvas("c","c",960,640)
c.SetBottomMargin(0.15)
c.SetRightMargin(0.25)
c.SetLeftMargin(0.11)

lat = ROOT.TLegend(0.76,0.62,0.99,0.89)
lat.SetBorderSize(0)
lat.SetTextFont(42)
lat.AddEntry(data,"Data - bkg","PEL")
lat.AddEntry(rata,"Prediction","L")
lat.AddEntry(ratae_nostat, "#pm Th. uncert.","F")
lat.AddEntry(ratae_noexp,"#pm MC stat. uncert.","F")
lat.AddEntry(ratae, "#pm expt.","F")

ratae.SetTitle("")
ratae.GetYaxis().SetTitleSize(0.06)
ratae.GetYaxis().SetTitleOffset(0.9)
ratae.GetXaxis().SetTitleSize(0.06)


ratae.SetFillColor(ROOT.kGray+2)
ratae_noexp.SetFillColor(ROOT.kGray+1)
ratae_nostat.SetFillColor(ROOT.kGray)
#ratae.GetYaxis().SetTitle("Z#rightarrow #mu#mu / W#rightarrow #mu#nu")
ratae.GetYaxis().SetTitle(ytitle)
ratae.SetMinimum(float(ymin))
ratae.SetMaximum(float(ymax))
ratae.GetXaxis().SetNdivisions(010)

ratae.Draw("E2")
ratae_noexp.Draw("E2same")
ratae_nostat.Draw("E2same")
rata.Draw("histsame")
data.Draw("PELsame")
lat.Draw()

tlat = ROOT.TLatex()
tlat.SetTextFont(42)
tlat.SetNDC()
tlat.DrawLatex(0.11,0.92,"#bf{CMS} #it{Preliminary}")
tlat.DrawLatex(0.44,0.92,lstr)
tlat.SetTextSize(0.04)
tlat.DrawLatex(0.14,0.83,clab)

c.SetTicky()
c.SetTickx()
c.RedrawAxis()
c.SaveAs("%s.pdf"%out)
c.SaveAs("%s.png"%out)

  

