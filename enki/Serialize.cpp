/*
 Enki - a fast 2D robot simulator
 Copyright © 2017 Jimmy Gouraud <jimmy.gouraud@etu.u-bordeaux.fr>
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Serialize.h"
#include "PhysicalEngine.h"



namespace Enki
{
    using namespace std;
    
    // ===== Practical function =====
    // ==============================

	std::vector<std::string> findSeparator(const string& str, const char& separator)
    {
		std::vector<std::string> s;
		int pos = -1;
		bool f = true;
		while(f || pos != std::string::npos){
			int deb = pos + 1;
			pos = str.find_first_of(separator, deb);
			s.push_back(str.substr(deb, pos - deb));
			f=false;
		}
		// delete blank end
		s.pop_back();
		return s;
    }

	

    void serializePoint(const Point &pos, ostringstream* oss)
    {
        *oss << pos.x << TYPE_SEPARATOR
        << pos.y << TYPE_SEPARATOR;
    }
    
	

}
