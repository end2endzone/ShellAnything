# Based on code from https://superuser.com/questions/1342323/how-can-i-directly-display-an-icon-with-a-known-dll-id

$code = @"
using System;
using System.Drawing;
using System.Runtime.InteropServices;

namespace System
{
    public class IconExtractor
    {

     public static Icon Extract(string file, int number, bool largeIcon)
     {
      IntPtr large;
      IntPtr small;
      ExtractIconEx(file, number, out large, out small, 1);
      try
      {
       return Icon.FromHandle(largeIcon ? large : small);
      }
      catch
      {
       return null;
      }
     }
     
     public static Int32 GetIconCount(string file)
     {
      IntPtr large;
      IntPtr small;
      return ExtractIconEx(file, -1, out large, out small, 1);
     }
     
     [DllImport("Shell32.dll", EntryPoint = "ExtractIconExW", CharSet = CharSet.Unicode, ExactSpelling = true, CallingConvention = CallingConvention.StdCall)]
     private static extern int ExtractIconEx(string sFile, int iIndex, out IntPtr piLargeVersion, out IntPtr piSmallVersion, int amountIcons);

    }
}
"@


function Resize-Image(
  [Parameter(Mandatory=$true)]
  [System.Drawing.Bitmap]$OldImage,
  [Parameter(Mandatory=$true)]
  [Int]$Width,
  [Parameter(Mandatory=$true)]
  [Int]$Height
)
{
  $OldHeight = $OldImage.Height
  $OldWidth = $OldImage.Width

  $Bitmap = New-Object System.Drawing.Bitmap $Width, $Height
  $graphics = [System.Drawing.Graphics]::FromImage($Bitmap)

  #Retrieving the best quality possible
  $graphics.CompositingMode = [System.Drawing.Drawing2D.CompositingMode]::SourceCopy
  $graphics.CompositingQuality = [System.Drawing.Drawing2D.CompositingQuality]::HighQuality
  $graphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
  $graphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality
  $graphics.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality

  # Draw
  $graphics.DrawImage($OldImage, $(New-Object -TypeName System.Drawing.Rectangle -ArgumentList 0, 0, $Width, $Height))
  
  #Cleanup
  $graphics.Dispose()
  $OldImage.Dispose()

  return $Bitmap
}

function ExtractIconToFile(
  [Parameter(Mandatory=$true)]
  [string]$InputPath,
  [Parameter(Mandatory=$true)]
  [string]$IconIndexNo,
  [Parameter(Mandatory=$true)]
  [string]$OutputPath
)
{
  If  (-not (Test-path -Path $InputPath -ErrorAction SilentlyContinue ) ) {
    Throw "Param InputPath [$InputPath] does not exist!"
  }
  
  # Extract icon from DLL
  Try {
    If ($InputPath.ToLower().Contains(".dll")) {
      Add-Type -TypeDefinition $code -ReferencedAssemblies System.Drawing
      $Icon = [System.IconExtractor]::Extract($InputPath, $IconIndexNo, $true)    
    } Else {
      [void][Reflection.Assembly]::LoadWithPartialName("System.Drawing")
      [void] [System.Reflection.Assembly]::LoadWithPartialName("System.Windows.Forms")
      $image = [System.Drawing.Icon]::ExtractAssociatedIcon("$($InputPath)").ToBitmap()
      $bitmap = new-object System.Drawing.Bitmap $image
      $bitmap.SetResolution(72,72)
      $icon = [System.Drawing.Icon]::FromHandle($bitmap.GetHicon())
    }
  } Catch {
    Throw "Error extracting icon from file $InputPath, $IconIndexNo"
  }

  # Convert to Bitmap
  $bitmap = $icon.ToBitmap()
  $icon.Dispose()

  # Save as PNG file
  Try {
    $bitmap.Save($OutputPath)
  } Catch {
    Throw "Error saving icon index $IconIndexNo to file $OutputPath"
  } Finally {
    $bitmap.Dispose()
  }

}

function GetCellFromIndex(
  [Parameter(Mandatory=$true)]
  [int]$Index,
  [Parameter(Mandatory=$true)]
  [int]$NumCols,
  [Parameter(Mandatory=$true)]
  [int]$NumRows
)
{
  $x = [int] ($Index % $NumCols)
  $y = [math]::floor(($Index / $NumCols))
  $point = New-Object System.Drawing.Point $x, $y
  return $point
}

function GetCellTopLeftPixel(
  [Parameter(Mandatory=$true)]
  [System.Drawing.Point]$Cell,
  [Parameter(Mandatory=$true)]
  [int]$CellWidth,
  [Parameter(Mandatory=$true)]
  [int]$CellHeight
)
{
  $point = New-Object System.Drawing.Point ($Cell.X*$CellWidth), ($Cell.Y*$CellHeight)
  return $point
}

function ProcessIconsInDllFile(
  [Parameter(Mandatory=$true)]
  [string]$DllPath,
  [Parameter(Mandatory=$true)]
  [string]$OutputPath
)
{
  If  (-not (Test-path -Path $DllPath -ErrorAction SilentlyContinue ) ) {
    Throw "Source file [$DllPath] does not exist!"
  }
  
  #Write-Output "Processing file $DllPath"

  # Count how many icons in DLL file. 
  Add-Type -TypeDefinition $code -ReferencedAssemblies System.Drawing
  $IconCount = [System.IconExtractor]::GetIconCount($DllPath)    
  Write-Output "Found $IconCount icons in file."
  
  # Constant
  Set-Variable GridCellWidth  -Option Constant -Value (10+32+10)
  Set-Variable GridCellHeight -Option Constant -Value (5+32+5+15+5)
  Set-Variable MaxIconPerRow  -Option Constant -Value 16

  # Compute grid size
  $NumCols = 16
  if ($IconCount -lt $MaxIconPerRow)
  {
    $NumCols = $IconCount
  }
  $NumRows = [int] [Math]::Truncate($IconCount / 16)
  if ($NumRows % 16)
  {
    $NumRows = $NumRows + 1
  } 
  if ($NumRows -eq 0)
  {
    $NumRows = 1
  } 
  $image_witdh = $NumCols * $GridCellWidth
  $image_height = $NumRows * $GridCellHeight
  Write-Output "Using a grid size of $NumCols x $NumRows, image size $image_witdh x $image_height (cells size $GridCellWidth x $GridCellHeight)."
  
  # Compute

  # Generate a bitmap
  $Bitmap = new-object System.Drawing.Bitmap $image_witdh, $image_height
  $BitmapGraphics = [System.Drawing.Graphics]::FromImage($Bitmap)

  $font = new-object System.Drawing.Font Consolas, 12
  $BrushBg = [System.Drawing.Brushes]::White
  $BrushFg = [System.Drawing.Brushes]::Black

  # Fill canvas with white
  $BitmapGraphics.FillRectangle($BrushBg, 0, 0, $Bitmap.Width, $Bitmap.Height)
  
  # For each icons
  #$IconIndexNo = 0
  #if ($true)
  for ($IconIndexNo = 0; $IconIndexNo -lt $IconCount; $IconIndexNo += 1)
  {
    # Read icon and save as png
    $TempPngFile = "$env:TEMP\temp.png"
    ExtractIconToFile -InputPath $DllPath -IconIndexNo $IconIndexNo -OutputPath $TempPngFile
    #Invoke-Item $TempPngFile

    # Load image
    $Icon = [System.Drawing.Image]::Fromfile($TempPngFile)
    #$Icon.Save("$home\\test.png")

    ## Force icons to always be 32x32
    #if ($Icon.Width -ne 32 -Or $Icon.Height -ne 32) {
    #  $Icon = Resize-Image -OldImage $Icon -Width 32 -Height 32
    #}

    # Resize icons to fit in 32x32 pixels (to fit our grid element)
    if ($Icon.Width -gt 32 -Or $Icon.Height -gt 32) {
      $Icon = Resize-Image -OldImage $Icon -Width 32 -Height 32
    }

    # Index to cell X,Y
    $cell = GetCellFromIndex -Index $IconIndexNo -NumCols $NumCols -NumRows $NumRows

    # Cell to pixel x,y
    $CellTopLeftPixel = GetCellTopLeftPixel -Cell $cell -CellWidth $GridCellWidth -CellHeight $GridCellHeight

    # Print progress
    #Write-Output "Processing icon at index $IconIndexNo. Cell $cell is displayed at topleft pixel $CellTopLeftPixel"

    # Draw cell boundary on canvas
    $CellRect = New-Object System.Drawing.Rectangle $CellTopLeftPixel.X, $CellTopLeftPixel.Y, $GridCellWidth, $GridCellHeight
    $BitmapGraphics.DrawRectangle($BrushFg, $CellRect)

    # Draw icon on canvas
    $IconTopLeftPixel = New-Object System.Drawing.Point ($CellTopLeftPixel.X + $GridCellWidth/2 - $Icon.Width/2), ($CellTopLeftPixel.Y + 5)
    $BitmapGraphics.DrawImage($Icon, $IconTopLeftPixel)

    # Draw text on canvas
    $text = $IconIndexNo
    $LabelSize = [System.Windows.Forms.TextRenderer]::MeasureText($text, $font)
    # Fix weird width calculated
    $LabelSize.Width = [int]((($LabelSize.Width*3)/3.5))

    $LabelTopLeftPixel = New-Object System.Drawing.Point ($CellTopLeftPixel.X + $GridCellWidth/2 - $LabelSize.Width/2), ($CellTopLeftPixel.Y + $GridCellHeight - $LabelSize.Height/2 - 10)
    $BitmapGraphics.DrawString($text, $font, $BrushFg, $LabelTopLeftPixel.X, $LabelTopLeftPixel.Y);

    # Draw text boundary on canvas
    $TextRect = New-Object System.Drawing.Rectangle $LabelTopLeftPixel.X, $LabelTopLeftPixel.Y, $LabelSize.Width, $LabelSize.Height
    $BitmapGraphics.DrawRectangle($BrushBg, $TextRect)

    $Icon.Dispose()
  }

  $BitmapGraphics.Dispose()

  # Note: Do not Dispose of System brushes.
  #$BrushBg.Dispose()
  #$BrushFg.Dispose()

  Try {
    $Bitmap.Save($OutputPath)
  } Catch {
    Throw "Error saving bitmap to file $OutputPath"
  } Finally {
    $Bitmap.Dispose()
  }

  $Bitmap = $null
  $BitmapGraphics = $null
  $BrushBg = $null
  $BrushFg = $null

  #Invoke-Item $OutputPath  
}
 
function UploadJUnitFile([string]$Path)
{
  $url = "https://ci.appveyor.com/api/testresults/junit/$env:APPVEYOR_JOB_ID"
  (New-Object "System.Net.WebClient").UploadFile($url, $path)
}
 
 
#Show OS Name
$WindowsOsName = (Get-WmiObject Win32_OperatingSystem | select -ExpandProperty Caption)
$WindowsOsName = $WindowsOsName.Replace('Microsoft ','')
$WindowsOsName = $WindowsOsName.Replace('Windows ','Win ')
Write-Output "Searching for icons on system: $WindowsOsName"

Add-Type -AssemblyName System.Drawing
Add-Type -AssemblyName System.Windows.Forms


$files = @(
  "$env:windir\system32\accessibilitycpl.dll"
  "$env:windir\System32\ieframe.dll"
  "$env:windir\System32\imageres.dll"
  "$env:windir\system32\netshell.dll"
  "$env:windir\system32\shell32.dll"
  "$env:windir\system32\urlmon.dll"
  "$env:windir\system32\user32.dll"
  "$env:windir\system32\wmploc.dll"
  "$env:windir\system32\wpdshext.dll"
  #"$env:windir\System32\compstui.dll"
  #"$env:windir\system32\ddores.dll"
  #"$env:windir\System32\mmcndmgr.dll"
  #"$env:windir\system32\mmres.dll"
  #"$env:windir\System32\moricons.dll"
  #"$env:windir\System32\pifmgr.dll"
  #"$env:windir\system32\powrprof.dll"
  #"$env:windir\system32\setupapi.dll"
  #"$env:windir\system32\shdocvw.dll"
  #"$env:windir\system32\Vault.dll"
  #"$env:windir\system32\wdc.dll"
  #"$env:windir\system32\wiashext.dll"
  #"$env:windir\System32\wmploc.DLL"
  #"$env:windir\system32\WorkFoldersRes.dll"
  #"$env:windir\system32\wusa.exe"
)
foreach($path in $files)
{
  $FileExists = [System.IO.File]::Exists($path)
  if (-Not ($FileExists)) {
    Write-Error "File '$path' does not exists."
    continue
  }   
  
  $file = Get-Item -Path $path
  $FilePath = $file.FullName
  $FileName = [System.IO.Path]::GetFileName($filePath)
  $OutputPath = $WindowsOsName + " - " + $FileName + " icons.png"

  Try {
    Write-Output "Processing icons from file '$FilePath' to '$OutputPath'"
    ProcessIconsInDllFile -DllPath $FilePath -OutputPath $OutputPath
  } Catch {
    Write-Error "Failed extractings icons from file '$FilePath' to '$OutputPath'"
    throw $_.Exception
    #Write-Error $_.Exception
  }

  Write-Output ""
}
