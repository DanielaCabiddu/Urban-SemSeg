# Set the folder path where your CSV files are located
$folderPath = "C:\Users\danie\Downloads\Tallinn_pointclouds\pointclouds"
$folderOutPath = "C:\Users\danie\Downloads\Tallinn_pointclouds\pointclouds-clean"

$ransacPath="C:\Users\danie\Devel\src\Urban-SemSeg\build-windows\ransac\Release\ransac.exe"

Get-ChildItem $folderPath -File | Foreach-Object {
    $fileName = $_.FullName
	$baseName = $_.Basename
	echo $baseName

	& $ransacPath -i $fileName -P -C -S -s 0.02 -o $folderOutPath\$baseName
}