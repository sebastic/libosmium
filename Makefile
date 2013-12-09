all: osmium.node

osmium.node:
	`npm explore npm -g -- pwd`/bin/node-gyp-bin/node-gyp --verbose build

clean:
	@rm -rf ./build
	rm -f lib/osmium.node

rebuild:
	@make clean
	@./configure
	@make

berlin-latest.osm.pbf:
	cd test/data
	wget http://download.geofabrik.de/europe/germany/berlin-latest.osm.pbf

test: berlin-latest.osm.pbf
	@PATH="./node_modules/mocha/bin:${PATH}" && NODE_PATH="./lib:$(NODE_PATH)" mocha -R spec --timeout 10000

indent:
	astyle --style=java --indent-namespaces --indent-switches --pad-header --lineend=linux --suffix=none src/\*pp

check: test

.PHONY: test indent
