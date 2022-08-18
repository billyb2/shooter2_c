function float_to_u8_array(f32) {
   let f32_arr = new Float32Array(1);
   f32_arr[0] = f32;

   let u8 = new Uint8Array(f32_arr.buffer);
   return Array.from(u8);
}

function uint_to_u8_array(u16) {
	let u16_arr = new Uint16Array(1);
	u16_arr[0] = u16;

	let u8 = new Uint8Array(u16_arr.buffer);
	return Array.from(u8);

}

var customMapFormat = {
    name: "Custom map format",
    extension: "custom",

    write: function(map, file_name) {
		let array = [];

		// Header info
		// Map size in pixels
		array = array.concat(float_to_u8_array(map.width * map.tileWidth));
		array = array.concat(float_to_u8_array(map.height * map.tileHeight));

		// Tile width and height
		array = array.concat(float_to_u8_array(map.tileWidth));
		array = array.concat(float_to_u8_array(map.tileHeight));

		let tilesets = map.tilesets;
		let num_tiles = 0;

		for (let i = 0; i < tilesets.length; i += 1) {
			let tileset = tilesets[i];
			num_tiles += tileset.tileCount;

		}

		// Add the tiles of each 
		array = array.concat(uint_to_u8_array(num_tiles));

		for (let i = 0; i < tilesets.length; i += 1) {
			let tileset = tilesets[i];
			let tiles = tileset.tiles;

			for (let z = 0; z < tiles.length; z += 1) {
				let tile = tiles[z];
				array = array.concat(uint_to_u8_array(tile.id));

				let beginning_of_file_name = tile.imageFileName.lastIndexOf("/");

				if (beginning_of_file_name == -1) {
					continue;

				}

				for (let j = beginning_of_file_name + 1; j < tile.imageFileName.length; j += 1) {
					array.push(tile.imageFileName[j].charCodeAt(0));

				}
				
				array.push(0);

			}

		}

        for (var i = 0; i < map.layerCount; i++) {
            var layer = map.layerAt(i);
            if (layer.isTileLayer) {
                for (y = 0; y < layer.height; y++) {
                    for (x = 0; x < layer.width; x++) {
						let tile = layer.tileAt(x, y);
						let cell = layer.cellAt(x, y);

						if (tile == null) {
							continue;

						}

						if (cell.tileId != -1) {
							array = array.concat(uint_to_u8_array(cell.tileId));

							let spawn = tile.property("spawn");

							if (spawn == true) {
								array.push(255);

							} else {
								array.push(0);

							}

							array = array.concat(float_to_u8_array(x));
							array = array.concat(float_to_u8_array(y));

						}
					}
                }
            }
        }

		let buffer = new Uint8Array(array);

		var file = new BinaryFile(file_name, BinaryFile.WriteOnly);

        file.write(buffer.buffer);
        file.commit();
    },
}

tiled.registerMapFormat("custom", customMapFormat);

