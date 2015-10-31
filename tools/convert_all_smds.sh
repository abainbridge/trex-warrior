#!bash

cd ../assets/shapes
for i in *.smd; do 
    shp=../../data/shapes/`echo $i | sed 's/\.smd/\.shp/'`
    echo Converting $i to $shp
    python ../../tools/smd_to_shp2.py $i $shp
done
