
#! MakeCplusplusHeader : Read the given file content into an output variable wrapping the file content in c++ comments.
#
# \arg:file The path of the file to read.
# \arg:output_variable The path to the output variable that contains the c++ comments.
#
function(MakeCplusplusHeader file output_variable)
  set(CPLUSPLUS_COMMENT_HEADER "/*********************************************************************************")
  set(CPLUSPLUS_COMMENT_FOOTER " *********************************************************************************/")
  
  # Read the file
  file(READ ${file} FILE_CONTENT)
  
  # Remove last \n at the end of the file.
  string(STRIP ${FILE_CONTENT} FILE_CONTENT)
  
  # Add " * " at the beginning of each new line
  string(REPLACE "\n" "\n * " FILE_CONTENT ${FILE_CONTENT})
  
  # Also add " * " for the first line
  set(FILE_CONTENT " * ${FILE_CONTENT}")
  
  # Add header and footer
  set(FILE_CONTENT "${CPLUSPLUS_COMMENT_HEADER}\n${FILE_CONTENT}\n${CPLUSPLUS_COMMENT_FOOTER}\n")
  
  # Debug
  #FILE(WRITE ${CMAKE_SOURCE_DIR}/MakeCplusplusHeader.h "${FILE_CONTENT}")
  
  # Copy formatted header to the calling scope
  set(${output_variable} "${FILE_CONTENT}" PARENT_SCOPE)
endfunction()
