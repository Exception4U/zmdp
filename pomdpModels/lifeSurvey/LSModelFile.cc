/********** tell emacs we use -*- c++ -*- style comments *******************
 $Revision: 1.2 $  $Author: trey $  $Date: 2006-06-12 18:44:58 $
   
 @file    LSModelFile.cc
 @brief   No brief

 Copyright (c) 2006, Trey Smith. All rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License"); you may
 not use this file except in compliance with the License.  You may
 obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 implied.  See the License for the specific language governing
 permissions and limitations under the License.

 ***************************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

#include "LSModelFile.h"
#include "zmdpCommonDefs.h"

using namespace std;

/**********************************************************************
 * DATA STRUCTURES
 **********************************************************************/

typedef std::map<std::string, std::string> ParamLookup;

/**********************************************************************
 * LOCAL HELPER FUNCTIONS
 **********************************************************************/

static void convertToDoubleVector(std::vector<double>& result,
				  const std::string& s)
{
  string::size_type p1, p2;
  p1 = 0;
  while (1) {
    p2 = s.find_first_of(" \t", p1);
    if (string::npos == p2) {
      result.push_back(atof(s.substr(p1).c_str()));
      break;
    } else {
      if (p1 != p2) {
	result.push_back(atof(s.substr(p1,(p2-p1)).c_str()));
      }
      p1 = p2+1;
    }
  }
}

static std::string getVal(const ParamLookup& params,
			  const std::string& paramName)
{
  typeof(params.begin()) i = params.find(paramName);
  if (params.end() == i) {
    fprintf(stderr, "ERROR: LSModelFile: preamble does not specify a value for parameter '%s'\n",
	    paramName.c_str());
    exit(EXIT_FAILURE);
  } else {
    return i->second;
  }
}

static std::string parseRow(const char* inRow,
			    int isOdd,
			    const std::string& fname,
			    int lnum)
{
  int n = strlen(inRow);
  std::string result(n/2+1, ' ');

  FOR (inIndex, n) {
    char c = inRow[inIndex];
    if (0 == ((isOdd + inIndex) % 2)) {
      /* even position */
      result[inIndex / 2] = c;
    } else {
      /* odd position -- expect a space character */
      if (' ' != c) {
	fprintf(stderr, "ERROR: %s: line %d: found an unexpected non-space character, map should have a checkerboard structure\n",
		fname.c_str(), lnum);
	printf("isOdd=%d inIndex=%d\n", isOdd, inIndex);
	exit(EXIT_FAILURE);
      }
    }
  }

  return result;
}

/**********************************************************************
 * LSGRID FUNCTIONS
 **********************************************************************/

LSGrid::LSGrid(void) :
  width(0),
  height(0),
  data(NULL)
{}

LSGrid::~LSGrid(void)
{
  if (NULL != data) {
    delete[] data;
    data = NULL;
  }
}

unsigned char LSGrid::getCellBounded(const LSPos& pos) const
{
  if (0 <= pos.x && pos.x < (int)width && 0 <= pos.y && pos.y < (int)height) {
    return getCell(pos);
  } else {
    return LS_OBSTACLE;
  }
}

bool LSGrid::getExitLegal(const LSPos& pos) const
{
  /* exiting is legal if there are no non-obstacle cells further to the
     east of this cell in the same row */
  for (int x=pos.x+1; x < (int)width; x++) {
    if (LS_OBSTACLE != getCell(LSPos(x, pos.y))) {
      return false;
    }
  }
  return true;
}

void LSGrid::writeToFile(FILE* outFile) const
{
  FOR (y0, height) {
    int y = height - y0 - 1;
    if ((y % 2) == 1) fputc(' ', outFile);
    for (unsigned int x=y/2; x < width; x++) {
      unsigned char cell = getCell(LSPos(x,y));
      char outCell;
      if (LS_OBSTACLE == cell) {
	outCell = ' ';
      } else {
	outCell = cell + 97;
      }
      fputc(outCell, outFile);
      fputc(' ', outFile);
    }
    fputc('\n', outFile);
  }
}

int LSGrid::getMaxCellValue(void)
{
  int maxValue = 0;
  FOR (y, height) {
    FOR (x, width) {
      int cell = getCell(LSPos(x,y));
      if (LS_OBSTACLE != cell) {
	maxValue = std::max(cell, maxValue);
      }
    }
  }
  maxValue++;
  return maxValue;
}

/**********************************************************************
 * LSMODEL FUNCTIONS
 **********************************************************************/

LSModelFile::LSModelFile(void) :
  startX(-1),
  startY(-1)
{}

void LSModelFile::readFromFile(const std::string& fname)
{
  FILE* modelFile = fopen(fname.c_str(), "r");
  if (NULL == modelFile) {
    fprintf(stderr, "ERROR: LSModelFile: couldn't open %s for reading: %s\n", fname.c_str(),
	    strerror(errno));
    exit(EXIT_FAILURE);
  }

  char lbuf[1024];
  char key[1024], value[1024];
  int lnum;

  /* read parameters from preamble */
  ParamLookup params;
  lnum = 0;
  while (NULL != fgets(lbuf, sizeof(lbuf), modelFile)) {
    lnum++;
    if (strlen(lbuf) <= 1) continue;
    if ('#' == lbuf[0]) continue;
    if ('-' == lbuf[0]) goto preambleDone;
    lbuf[strlen(lbuf)-1] = '\0'; // truncate newline
    if (2 != sscanf(lbuf, "%s %[-0-9.eE \t]", key, value)) {
      fprintf(stderr, "ERROR: %s: line %d: syntax error, expected '<key> <value>'\n",
	      fname.c_str(), lnum);
      exit(EXIT_FAILURE);
    }
    params[key] = value;
  }
  fprintf(stderr, "ERROR: %s: reached EOF while still parsing preamble\n",
	  fname.c_str());
  exit(EXIT_FAILURE);
 preambleDone:

  /* store parameters */
  startX = atoi(getVal(params, "startX").c_str());
  startY = atoi(getVal(params, "startY").c_str());
  convertToDoubleVector(regionPriors, getVal(params, "regionPriors"));

  /* read map data into intermediate data structure */
  std::vector<std::string> rows;
  while (NULL != fgets(lbuf, sizeof(lbuf), modelFile)) {
    lnum++;
    if (strlen(lbuf) <= 1) {
      if (feof(modelFile)) {
	break;
      } else {
	fprintf(stderr, "ERROR: %s: line %d: found unexpected zero-length line in map\n",
		fname.c_str(), lnum);
	exit(EXIT_FAILURE);
      }
    }
    lbuf[strlen(lbuf)-1] = '\0'; // truncate trailing newline
    rows.push_back(parseRow(lbuf, rows.size() % 2, fname, lnum));
  }

  /* calculate dimensions of map */
  int maxRowWidth = 0;
  FOR (y0, rows.size()) {
    int y = rows.size() - y0 - 1;
    maxRowWidth = std::max(maxRowWidth, (int) (y/2 + rows[y0].size()));
  }

  /* allocate final map data structure */
  grid.width = maxRowWidth;
  grid.height = rows.size();
  grid.data = new unsigned char[grid.width*grid.height];
  FOR (y, grid.height) {
    FOR (x, grid.width) {
      grid.setCell(LSPos(x,y),LS_OBSTACLE);
    }
  }

  /* convert from immediate to final data structure */
  FOR (y0, rows.size()) {
    int y = rows.size() - y0 - 1;
    FOR (x0, rows[y0].size()) {
      int x = y/2 + x0;
      char c = rows[y0][x0];
      unsigned char cell;
      if (' ' == c) {
	cell = LS_OBSTACLE;
      } else if (97 <= c && c <= 122) {
	cell = c-97;
      } else {
	fprintf(stderr, "ERROR: %s: while parsing map, found character '%c', expected space or a-z.\n",
		fname.c_str(), c);
	exit(EXIT_FAILURE);
      }
      grid.setCell(LSPos(x, y), cell);
    }
  }

  if ((int)regionPriors.size() != grid.getMaxCellValue()) {
    int n = grid.getMaxCellValue();
    fprintf(stderr, "ERROR: %s: number of regionPriors (%lu) should match number of regions in map (a-%c=%d)\n",
	    fname.c_str(), regionPriors.size(), (char) ((n-1)+97), n);
    exit(EXIT_FAILURE);
  }
}

void LSModelFile::writeToFile(FILE* outFile) const
{
  fprintf(outFile, "startX %d\n", startX);
  fprintf(outFile, "startY %d\n", startY);

  fprintf(outFile, "regionPriors ");
  FOR (i, regionPriors.size()) {
    fprintf(outFile, "%lf ", regionPriors[i]);
  }
  fprintf(outFile, "\n");

  fprintf(outFile, "---\n");
  grid.writeToFile(outFile);
}

/***************************************************************************
 * REVISION HISTORY:
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2006/06/12 18:12:08  trey
 * renamed LSModel to LSModelFile; minor updates
 *
 * Revision 1.1  2006/06/11 14:37:38  trey
 * initial check-in
 *
 *
 ***************************************************************************/