numbatDir=$(pwd)
workDir=$(mktemp -d)
cd $workDir

mkdir build
cd build
cmake -Ddebug=OFF $numbatDir
make -j 8 numbat
cd ..


files=$(find $numbatDir/test | grep \\.nbt)
for file in $files; do
	echo ""
	echo "Building: $file"
	build/numbat $file
done

du -sh build
cd $numbatDir
rm -rf $workDir