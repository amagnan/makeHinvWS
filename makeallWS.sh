cmsenv

for year in 2017 2018;
do
    for categ in MTRC VTRC MTRF VTRF;
    do

	cd test_df_${categ}_${year}_2020v1
	root -b "../makeallWS.C(\"${year}\",\"${categ}\")"
	root -b "../makeSWS.C(\"${year}\",\"${categ}\")"
	cd ../
    done
done

