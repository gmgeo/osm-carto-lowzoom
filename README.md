# OSM Carto Low Zoom

A C++ program for use with the [Osmium library](https://github.com/osmcode/libosmium) to extract data from OSM PBF files necessary to
render low zoom (< z8) tiles of [openstreetmap-carto](https://github.com/gravitystorm/openstreetmap-carto).
The extract is written to a OSM PBF file.

## Tags

The following tags are included.

Relations:
* `boundary=administrative` and `type=boundary` and `admin_level=2|3|4`
* `boundary=nature_reserve`
* `boundary=national_park`

Ways:
* `highway=motorway`
* `highway=trunk`
* `railway=rail`
* `natural=glacier`
* `natural=water`

Nodes:
* `place=city`
* `place=town`

## Compiling

Make sure that you have setup your development environment. There are various tutorials out there how to do that.
Install [dependencies](https://github.com/osmcode/libosmium/wiki/Libosmium-dependencies) that are required by Osmium, namely zlib and Boost iterator.
Checkout a copy of Osmium from GitHub and place it within this repository or any other location you want.

Compile with:
``g++ -Ipath/to/libosmium/include -std=c++11 -o osm-carto-lowzoom osm-carto-lowzoom.cpp -pthread -lz``

If you want to include debug symbols add the ``-g`` option.

## Usage

Run the program with ``./osm-carto-lowzoom input.osm.pbf output.osm.pbf``.

## License

Available under the MIT License. See [LICENSE.txt](https://github.com/gmgeo/extract-admin/blob/master/LICENSE.txt) for details.
