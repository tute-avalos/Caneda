/***************************************************************************
 * Copyright (C) 2007 by Gopala Krishna A <krishna.ggk@gmail.com>          *
 *                                                                         *
 * This is free software; you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2, or (at your option)     *
 * any later version.                                                      *
 *                                                                         *
 * This software is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this package; see the file COPYING.  If not, write to        *
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,   *
 * Boston, MA 02110-1301, USA.                                             *
 ***************************************************************************/

#include "units.h"

namespace Caneda
{
   // Unit conversion array for length.
   double lengthConversionTable[7][7] = {
      { 1.0,       2.54e-3,   2.54e-2,   2.54e-5,   25.4,     1.e-3,   1./12000},
      {1./2.54e-3,    1.0,     10.0,      1.e-2,    1.e4,   1./2.54,   1./30.48},
      {1./2.54e-2,  1./10.,     1.0,       1.e-3,   1.e3,   1./25.4,   1./304.8},
      {1./2.54e-5,    1.e2,    1.e3,         1.0,   1.e6,  1./2.54e-2, 1./0.3048},
      {1./25.4,      1.e-4,   1.e-3,       1.e-6,    1.0,  1./2.54e4,  1./3.048e5},
      {1.e3,          2.54,    25.4,     2.54e-2, 2.54e4,        1.0,    1./12.},
      {1.2e4,        30.48,   304.8,      0.3048, 3.048e5,      12.0,       1.0}
   };

   // Unit conversion array for frequencies.
   double frequencyConversionTable[4][4] = {
      { 1.0,     1.e9,     1.e6,      1.e3},
      { 1.e-9,   1.0,      1.e-3,     1.e-6},
      { 1.e-6,   1.e3,     1.0,       1.e-3},
      { 1.e-3,   1.e6,     1.e3,      1.0}
   };

   // Unit conversion array for resistances.
   double resistanceConversionTable[2][2] = {
      {1.0,    1.e-3},
      {1.e3,   1.0}
   };

   // Unit conversion array for angles.
   double angleConversionTable[2][2] = {
      {1.0,         M_PI/180.0},
      {180.0/M_PI,         1.0}
   };

   QString toString(FrequencyUnits f)
   {
      return Caneda::freqList[int(f)];
   }
   QString toString(LengthUnits l)
   {
      return Caneda::lenList[int(l)];
   }
   QString toString(ResistanceUnits r)
   {
      return Caneda::resList[int(r)];
   }
   QString toString(AngleUnits a)
   {
      return Caneda::angleList[int(a)];
   }

   QString toString(int u, UnitType t)
   {
      switch(t) {
         case Frequency:
            return toString(FrequencyUnits(u));
         case Length:
            return toString(LengthUnits(u));
         case Resistance:
            return toString(ResistanceUnits(u));
         case Angle:
            return toString(AngleUnits(u));
         default:
            return QString();
      };
   }

   double convert(double value, Caneda::UnitType ut, int fromUnit, int toUnit)
   {
      double cnv = value;
      switch(ut)
      {
         case Frequency:
            cnv *= frequencyConversionTable[int(fromUnit)][toUnit];
            break;
         case Length:
            cnv *= lengthConversionTable[int(fromUnit)][toUnit];
            break;
         case Resistance:
            cnv *= resistanceConversionTable[int(fromUnit)][toUnit];
            break;
         case Angle:
            cnv *= angleConversionTable[int(fromUnit)][toUnit];
            break;
         default:
            break;
      };
      return cnv;
   }

   int toInt(const QString& unit)
   {
      if(unit == "NA")
         return None;
      for(int i=0; i<freqList.size(); ++i)
      {
         if(unit == freqList[i])
            return i;
      }
      for(int i=0; i<lenList.size(); ++i)
      {
         if(unit == lenList[i])
            return i;
      }
      for(int i=0; i<resList.size(); ++i)
      {
         if(unit == resList[i])
            return i;
      }
      for(int i=0; i<angleList.size(); ++i)
      {
         if(unit == angleList[i])
            return i;
      }
      return None;
   }

} // namespace Caneda
