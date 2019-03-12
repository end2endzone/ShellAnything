Param(
  [Parameter(Mandatory=$True)]
  [string]$Path
)
 
function IsJUnitFile([string]$Path)
{
  $JUnitSignature = "<testsuites tests="
  $content = Get-Content $Path | Out-String
  if ($content.Contains($JUnitSignature))
  {
    return $true
  }
  return $false
}
 
function UploadJUnitFile([string]$Path)
{
  $url = "https://ci.appveyor.com/api/testresults/junit/$env:APPVEYOR_JOB_ID"
  (New-Object "System.Net.WebClient").UploadFile($url, $path)
}
 
# Search for all xml files in $Path
Write-Output "Search for all junit test results (*.xml files) in '$Path'..."
$files = Get-ChildItem -Path "$Path" -Filter *.xml -Recurse -ErrorAction SilentlyContinue -Force
 
foreach($file in $files)
{
  $FilePath = $file.FullName
  if (IsJUnitFile -Path $FilePath)
  {
    Write-Output "Uploading '$FilePath'..."
    UploadJUnitFile -Path $FilePath
  }
  else
  {
    Write-Output "Skipped '$FilePath'."
  }
}
