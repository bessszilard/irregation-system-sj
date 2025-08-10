mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make && ./simulation/simulation
./simulation/simulation /home/szilard/git/irregation-system-sj/simulation/results/simulation.csv /home/szilard/git/irregation-system-sj/simulation/config/commands.json