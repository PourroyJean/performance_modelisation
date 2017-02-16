#!/usr/bin/env bash

sudo opcontrol --dump
#T=$(date +%y%m%d%H%M%S).$(uname –n)

# COMMAND
#./GRE.oprofile.sh
#grep -v "^ " op2 | more
#objdump -d  bsmbench_compute_1cpu  > OBJ.18
#./dmlv3 OBJ.18 op2 > op4.suivant


#Patrick tool
opreport    >op1
opreport -d >op2



#Afficher les executables > threshold %
#/usr/bin/opreport --long-filenames --threshold 10

#Avec nom de la fonction
#opreport --long-filenames --symbols --threshold 10


#Afficher les fonctions associe a l'image
#/usr/bin/opreport --long-filenames image:bsmbench_compute_1cpu -l

#Annoter le code
#/usr/bin/opannotate --threshold 1 image:bsmbench_compute_1cpu --source bsmbench_compute_1cpu.annc


#sudo opcontrol --dump ; opreport --treshold 1
#1062163  86.5114  1091736  87.1027  spinor_field_sqnorm_f
#1101437   9.0573  1036146  15.0082  spinor_field_mulc_add_assign_f

