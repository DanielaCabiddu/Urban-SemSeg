# Set the folder path where your XYX files are located
$folderPath = "C:\Users\danie\Downloads\Tallinn_pointclouds\pointclouds"

Workflow Run-RANSAC {

	$outPath = "C:\Users\danie\Downloads\Tallinn_pointclouds\pointclouds-test"
	$ransacExe="C:\Users\danie\Devel\src\Urban-SemSeg\build-windows\ransac\Release\ransac.exe"

	$Files = Get-Content 'files.txt'

	ForEach -Parallel ($File in $Files) 
	{	
		InlineScript
		{
			$fileName = $Using:File
			$folderOutPath = $Using:outPath
			$ransacPath = $Using:ransacExe
			
			$ff = Get-Item $fileName

			$dirfileName = $folderOutPath + "\" + $ff.Basename + ".dir"

			$logfileName = $dirfileName + "\" + $ff.Basename + ".log"
			$errfileName = $dirfileName + "\" + $ff.Basename + ".err"

			echo "Running $fileName : $dirfileName"

			mkdir $dirfileName
			& $ransacPath -i $fileName -P -C -S -s 0.02 -o $dirfileName 2>$errfileName 1>$logfileName

			echo "Running $fileName : $dirfileName : DONE"

		}

	}

}

& .\dir.bat $folderPath 1>files.txt

Run-RANSAC