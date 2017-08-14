/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _TYPE__H__
#define _TYPE__H__

#include <vector>

namespace push {

class Type;
class Env;
std::ostream& operator<<(std::ostream& os, const Type& tp);
    
class Type {
    std::vector<int> type;
    unsigned _start;
    
    public:
	
        Type(size_t len=0) : type(len) { postc(); }
        Type(size_t len, size_t which, int value = 1) : type(len) {type[which]=value; postc(); }
        
        typedef std::vector<int> TypeVec;
        
        const TypeVec& get() const { return type; }
        
        Type& operator+=(const Type& org) { 
            Type other = make_equal_length(org);
            for (unsigned i = 0; i < type.size(); ++i) {
                type[i] += other.type[i];
            }
            shorten();
            return *this; 
        }
        
        Type& operator-=(const Type& org) {
            Type other = make_equal_length(org);
            for (unsigned i = 0; i < type.size(); ++i) {
                type[i] -= other.type[i];
            }
            shorten();
            return *this; 
        }
        
        bool operator==(const Type& other) const;
        bool can_pop_from(const Env& env) const;

        int operator[](unsigned i) const { return i < type.size()? type[i] : 0; }
        
        void abs();
        
        Type& operator*=(int val) {
            for (unsigned i = 0; i < type.size(); ++i) {
                type[i] *= val;
            }
            return *this;
        }

        bool is_positive() const {
            for (unsigned i = 0; i < type.size(); ++i) {
                if (type[i] < 0) return false;
            }
            return true;
        }
	
    private:
        Type make_equal_length(const Type& org) {
            if (type.size() == org.type.size()) return org;
            
            size_t len = std::max(type.size(), org.type.size());
            
            type.resize(len,0);
            
            if (org.type.size() != len) {
                Type org2 = org;
                org2.type.resize(len,0);
                return org2;
            }
            return org;
        }

        void shorten() { while (type.size() && type.back()==0) type.pop_back(); postc(); }
        void postc() {
            for (_start = 0; _start < type.size(); ++_start) {
                if (type[_start] > 0) break;
            }
        }
};

inline Type operator+(const Type& a, const Type& b) { return Type(a) += b; }
inline Type operator-(const Type& a, const Type& b) { return Type(a) -= b; }
inline Type abs(const Type& a) { Type retval(a); retval.abs(); return retval; }

inline Type operator*(const Type& a, int b) { return Type(a) *= b; }
inline Type operator*(int b, const Type& a) { return Type(a) *= b; }


} // namespace push

#endif
