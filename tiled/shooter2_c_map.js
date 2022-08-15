function float_to_u8_array(f32) {
   let f32_arr = new Float32Array(1);
   f32_arr[0] = f32;

   let u8 = new Uint8Array(f32_arr.buffer);
   return Array.from(u8);
}

var customMapFormat = {
    name: "Custom map format",
    extension: "custom",

    write: function(map, file_name) {
        var m = {
            width: map.width,
            height: map.height,
            layers: []
        };

		let array = [];

        for (var i = 0; i < map.layerCount; i++) {
            var layer = map.layerAt(i);
            if (layer.isTileLayer) {
                var rows = [];
                for (y = 0; y < layer.height; y++) {
                    var row = [];
                    for (x = 0; x < layer.width; x++) {
						let cell = layer.cellAt(x, y);

						if (cell.tileId != -1) {
							array.push(cell.tileId);
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

