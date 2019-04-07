' Usage:
'   arg 0 is oldstr
'   arg 1 is newstr
'   arg 2 is input file
'
strFile=WScript.Arguments.Item(2)
strOld=WScript.Arguments.Item(0)
strNew=WScript.Arguments.Item(1)

Set objFSO = CreateObject("Scripting.FileSystemObject")

Dim linestr

'Read file content
ReDim fileContent(-1)
Set objFileToRead = objFSO.OpenTextFile(strFile,1)
i = 0
do while not objFileToRead.AtEndOfStream
  linestr = objFileToRead.ReadLine()
  'WScript.Echo "Reading name [" & linestr & "] with a length of " & len(linestr)
  if (len(linestr) > 0) then
    ReDim Preserve fileContent( UBound(fileContent) + 1)
    fileContent(i) = linestr
    i = i + 1
  end if
loop
objFileToRead.Close
Set objFileToRead = Nothing


'Overwrite existing file...
Set objFileToWrite = objFSO.CreateTextFile(strFile,True)
'For each lines of content
For i = LBound( fileContent ) To UBound( fileContent )
  linestr = fileContent( i )
  linestr=Replace(linestr,strOld,strNew)
  objFileToWrite.Write linestr & vbCrLf
Next
objFileToWrite.Close
