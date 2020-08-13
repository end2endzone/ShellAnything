/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

#include "Wildcard.h"

namespace shellanything
{

  bool IsWildcard(char c)
  {
    if (c == '*' || c == '?')
      return true;
    return false;
  }

  int getNextWildcardPosition(const IntList & iPositions, int iSourceWildcardPosition)
  {
    for(unsigned int i=0; i<iPositions.size(); i++)
    {
      const int & pos = iPositions[i];
      if (pos == iSourceWildcardPosition)
      {
        if (i+1 < iPositions.size())
        {
          return iPositions[i+1];
        }
        return -1; //requested wildcard is the last one
      }
    }
    return -1; //wildcard position not found
  }

  void FindWildcardCharacters(const char * iString, int iOffset, IntList & oIndexes)
  {
    oIndexes.clear();
    int len = (int)std::string(iString).size();
    for(int i=iOffset; i<len; i++)
      if ( IsWildcard(iString[i]) )
        oIndexes.push_back(i);
  }

  bool HasWildcardCharacters(const char * iValue)
  {
    IntList positions;
    FindWildcardCharacters(iValue, 0, positions);
    return positions.size() > 0;
  }

  bool WildcardSolve( const char * iWildcard,
                      const char * iValue,
                      const IntList & iWildcardPositions,
                      const int iIndexWild,
                      const int iIndexValue,
                      WildcardList & oList                     )
  {
    int indexWild = iIndexWild;
    int indexValue = iIndexValue;
    int wildLen   = (int)std::string(iWildcard).size();
    int valueLen  = (int)std::string(iValue).size();
  
    //while wildcard and value not fully solved
    while (indexWild < wildLen || indexValue < valueLen)
    {
      const char & cWild  = iWildcard[indexWild];
      const char & cValue = iValue[indexValue];
      if ( !IsWildcard(cWild) )
      {
        if (cWild != cValue)
          return false; //characters don't match!

        //next char
        indexWild++;
        indexValue++;
      }
      else
      {
        //cWild is wildcard
        if (cWild == '?')
        {
          //save wildcard resolve information
          WILDCARD w;
          w.character = cWild;
          w.index = indexWild;
          w.value = cValue;
          oList.push_back(w);

          //next char
          indexWild++;
          indexValue++;
        }
        else if (cWild == '*')
        {
          //if wildcard character * is the last one
          if (indexWild+1 >= wildLen)
          {
            //automatically match the end of the string

            //save wildcard resolve information
            WILDCARD w;
            w.character = cWild;
            w.index = indexWild;
            w.value = &cValue;
            oList.push_back(w);

            //next char
            indexWild++;
            indexValue += (int)w.value.size();
          }
          else
          {
            //wildcard character * is not the last one

            //compute all possibilities that can fit in * and then use recursion to check if it can be resolved
            //compute the possibilities in the from the longest to the shortest ("") for optimizing the replacement string
            //since all wildcard caracters must be mapped, do not compute possibilities beyond the next wildcard character


            //compute next character
            const char & nextWild = (&cWild)[1];

            int nextWildcardPosition = getNextWildcardPosition(iWildcardPositions, indexWild);

            //compute fixed characters between this wildcard and the last one (of the end of the file)
            //ie: fgh in abc*fgh?j or abc*fgh
            std::string fixedCharacters = &nextWild;
            if (nextWildcardPosition != -1)
            {
              //there is at least another wildcard character after this one
              fixedCharacters = &nextWild;
              size_t size = nextWildcardPosition - indexWild;
              fixedCharacters.resize(size);
            }

            //compute maximum length of replacementString
            int remainingCharactersInValue = valueLen-indexValue;
            int replacementStringMaxLength = remainingCharactersInValue-(int)fixedCharacters.size();

            //compute all possibilities that can fit in * with a length in [0,replacementStringMaxLength]
            for(int length=replacementStringMaxLength; length>=0; length--)
            {
              //assuming replacement string is the right one

              std::string replacementString = &cValue;
              replacementString.resize(length);

              WildcardList tmpList = oList;

              //save wildcard resolve information
              WILDCARD w;
              w.character = cWild;
              w.index = indexWild;
              w.value = replacementString;
              tmpList.push_back(w);

              //next char
              int tmpIndexWild = indexWild+1;
              int tmpIndexValue = indexValue + (int)w.value.size();

              //execute recursive call
              bool solved = WildcardSolve(iWildcard, iValue, iWildcardPositions, tmpIndexWild, tmpIndexValue, tmpList);
              if (solved)
              {
                //solved!
                //refresh oList
                oList = tmpList;
                return true;
              }
            }

            //all possibilities were checked
            //unable to solve * wildcard
            return false;
          }
        }
      }
    }

    if (indexWild == wildLen && indexValue == valueLen)
      return true; //solved
    if ((indexWild == wildLen && indexValue < valueLen) ||
        (indexWild == wildLen && indexValue < valueLen)     )
      return false; //reached the end of wildcard or the end of value
    return false; //???
  }

  bool WildcardSolve(const char * iWildcard, const char * iValue, WildcardList & oList)
  {
    oList.clear();
    int indexWild = 0;
    int indexValue = 0;

    //find all wildcard character positions
    IntList wildcardPositions;
    FindWildcardCharacters(iWildcard, 0, wildcardPositions);

    //if no wildcard character, strings must be equal
    if (wildcardPositions.size() == 0)
      return (std::string(iWildcard) == std::string(iValue));

    //solve wildcards
    return WildcardSolve(iWildcard, iValue, wildcardPositions, indexWild, indexValue, oList);
  }

  bool WildcardMatch(const char * iWildcard, const char * iValue)
  {
    WildcardList dummy;
    return WildcardSolve(iWildcard, iValue, dummy);
  }

  WildcardGroupList FindWildcardGroups(const char * iString)
  {
    WildcardGroupList groups;

    IntList positions;
    FindWildcardCharacters(iString, 0, positions);
    for(size_t i=0; i<positions.size(); i++)
    {
      int & pos = positions[i];
      char c = iString[pos];

      //add current wildcard type to list
      bool found = false;
      for(size_t j=0; j<groups.size(); j++)
      {
        if (groups[j].character == c)
        {
          groups[j].indexes.push_back(pos);
          found = true;
        }
      }
      if (!found)
      {
        //create a new one
        WILDCARD_GROUP g;
        g.character = c;
        g.indexes.push_back(pos);
        groups.push_back(g);
      }
    }

    return groups;
  }

} //namespace shellanything
