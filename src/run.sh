make clean
make
rm output.txt

printf "custom\n\n" >> output.txt
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --custom >> output.txt
printf "\n" >> output.txt
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --custom >> output.txt
printf "\n" >> output.txt
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --custom >> output.txt
printf "\n" >> output.txt
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --custom >> output.txt
printf "\n" >> output.txt
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --custom >> output.txt
printf "\n" >> output.txt
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --custom >> output.txt
printf "\n" >> output.txt

printf "tournament:9:10:10\n\n" >> output.txt
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --tournament:9:10:10 >> output.txt
printf "\n" >> output.txt
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --tournament:9:10:10 >> output.txt
printf "\n" >> output.txt
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --tournament:9:10:10 >> output.txt
printf "\n" >> output.txt
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --tournament:9:10:10 >> output.txt
printf "\n" >> output.txt
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --tournament:9:10:10 >> output.txt
printf "\n" >> output.txt
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --tournament:9:10:10 >> output.txt
printf "\n" >> output.txt

printf "gshare:13\n\n" >> output.txt
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --gshare:13 >> output.txt
printf "\n" >> output.txt
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --gshare:13 >> output.txt
printf "\n" >> output.txt
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --gshare:13 >> output.txt
printf "\n" >> output.txt
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --gshare:13 >> output.txt
printf "\n" >> output.txt
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --gshare:13 >> output.txt
printf "\n" >> output.txt
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --gshare:13 >> output.txt
printf "\n" >> output.txt