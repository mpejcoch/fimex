/*
  Fimex, test/testinghelpers.cc

  Copyright (C) 2019 met.no

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
  NORWAY
  email: diana@met.no

  Project Info:  https://wiki.met.no/fimex/start

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
  USA.
*/


// prevent #include <boost/test/unit_test.hpp> from testinghelpers.h here
#define FIMEX_TESTINGHELPERS_NO_BOOST
#include "testinghelpers.h"

#include <stdexcept>
#include <fstream>

using std::string;

namespace {
const string top_src(TOP_SRCDIR);

const string src_share_etc(TOP_SRCDIR "/share/etc/");
const string src_test(TOP_SRCDIR "/test/");

const string extra_data_dir(TEST_EXTRADATA_DIR "/");
} // namespace

namespace MetNoFimex {

const std::string& topSrcDir()
{
    return top_src;
}

bool exists(const std::string& path)
{
    return std::ifstream(path.c_str()).is_open();
}

void remove(const std::string& path)
{
    ::remove(path.c_str());
}

string require(const std::string& path)
{
    if (!exists(path))
        throw std::runtime_error("no such file: '" + path + "'");
    return path;
}

size_t file_size(const std::string& path)
{
    std::ifstream f(path, std::ios::binary);
    const size_t begin = f.tellg();
    f.seekg(0, std::ios::end);
    const size_t end = f.tellg();
    return end - begin;
}

string pathShareEtc(const std::string& filename)
{
    return require(src_share_etc + filename);
}

string pathTest(const std::string& filename)
{
    return require(src_test + filename);
}

string pathTestExtra(const std::string& filename)
{
    return require(extra_data_dir + filename);
}

bool hasTestExtra()
{
    return exists(extra_data_dir + "flth00.dat");
}

void copyFile(const std::string& from, const std::string& to)
{
    std::ifstream  src(from.c_str());
    std::ofstream  dst(to.c_str());
    dst << src.rdbuf();
}

} // namespace MetNoFimex

#ifndef HAVE_BOOST_UNIT_TEST_FRAMEWORK
// no / old boost testframework
int main(int argc, char* args[]) {
}
#endif // !HAVE_BOOST_UNIT_TEST_FRAMEWORK
