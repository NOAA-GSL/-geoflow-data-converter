# INPUT JSON FILE

## Overview
The GeoFLOW data converter application reads in a `.json` file that contains configuration information needed for running the converter. This information includes items like input and output directory names, how many timesteps to run, specs needed to write the UGRID `.nc` files and more. 

Below is a template input `.json` file. In the template, the word `---EDIT---` has been prepended to any line that must be edited for your input file. All other lines should not be edited. A few notes:
- Although descriptions and comments don't actually need to be edited, they have been marked as such for correctness.
- For the field variables (such as `dtotal` and `v1` in the template example), the value for the key `units` can optionally be left as an empty string (i.e., in the `variables` array > `attributes` array > object starting with `"name": "units"`).
- For most spherical datasets, the grid variables `mesh_node_x`, `mesh_node_y`, `meshLayers` and `mesh_depth` can be left alone and only need to be edited if the units are different what is indicated in the template file.
- For box datasets, the value of `units` for variables `mesh_node_x` and `mesh_node_y` should be changed from longitude and latitude to meters (or some other appropriate metric).

Consider making a copy of the template below, making the required edits, and then deleting all instances of the word `---EDIT---`. Alternatively, make a copy of one of the input `.json` files in the `test-data` directory and edit that file. To add another field variable, simply copy one of the field variables in the `variables` array and make edits to it.

## Definitions
- **input_dir**: Path to input directory containing the GeoFLOW-formatted grid and field variable files
- **output_dir**: Path to output directory where the resulting `.nc` files in the UGRID format will be written
- **data_type**: Data type (for example `GDOUBLE` or `GFLOAT`)
- **num_timesteps**: Number of timesteps to convert
- **is_spherical**: True if dataset is spherical, False if dataset is box
- **print_nodes**: Print a sorted list (from bottom to top wrt to GeoFLOW element ID and 2D mesh layers) of nodes where each node contains the x,y,z grid values and the corresponding field variable values).
- **write_separate_var_files**: True if writing each field variable to a separate file, False if writing all field variables (for a given timestep) to one file
- **grid_filenames**: Names of the x,y,z input GeoFLOW grid filenames
- **field_variable_root_names**: Root names of the field variable files (for example, if a dataset contains field variable files for `dtotal` with `dtotal.000000.out` and `dtotal.000001.out`, the root name is `dtotal`)

## Template
```
{
"output_format": "UGRID",
---EDIT--- "dataset_description": "Example of a 3D GeoFLOW dataset with multiple element layers.",
---EDIT--- "input_dir": "test-data/input-data-3D",
---EDIT--- "output_dir": "output-data",
---EDIT--- "data_type": "GDOUBLE",
---EDIT--- "num_timesteps": 1,
---EDIT--- "is_spherical": true,
---EDIT--- "print_nodes": false,
---EDIT--- "write_separate_var_files": true,
---EDIT--- "grid_filenames":
{
    "x": "xgrid.000000.out",
    "y": "ygrid.000000.out",
    "z": "zgrid.000000.out"
},
"grid_variable_names":
[
    "mesh_node_x",
    "mesh_node_y",
    "mesh_depth"
],
---EDIT--- "field_variable_root_names":
[
    "dtotal",
    "v1"
],
"dimensions": 
[
    {
        "name": "nMeshNodes",
        "value": 0,
        "comment": "number of nodes per mesh layer (determined at runtime)" 
    },
    {
        "name": "nMeshFaces",
        "value": 0,
        "comment": "number of faces per mesh layer (determined at runtime)"
    },
    {
        "name": "nFaceNodes",
        "value": 4,
        "comment": "number of nodes per face"
    },
    {
        "name": "meshLayers",
        "value": 0,
        "comment": "number of mesh layers in the volume (determined at runtime)"
    },
    {
        "name": "time",
        "value": 1,
        "comment": "file contains one timestep worth of data"
    }
],

"variables" :
[
    {
        "comment": "Timestamp",
        "name": "time",
        "type": "data_type",
        "args":
        [
            "time"
        ],
        "attributes":
        [
            {
                "name": "long_name",
                "value": "time",
                "comment": ""
            },
            {
                "name": "units",
                "value": "second",
                "comment": "TBD, but an example is: hours since YYYY-MM-DD HH:MM:SS"
            },
            {
                "name": "calendar",
                "value": "",
                "comment": "TBD but an example is: standard"
            },
            {
                "name": "axis",
                "value": "T",
                "comment": "required attribute in VAPOR" 
            }
        ]
    },
    {
        "comment": "Mesh topology described by a \\“dummy\\” variable",
        "name": "mesh",
        "type": "GINT",
        "args":
        [
        ],
        "attributes":
        [
            {
                "name": "cf_role",
                "value": "mesh_topology",
                "comment": ""
            },
            {
                "name": "long_name",
                "value": "Topology data of 2D unstructured mesh.",
                "comment": ""
            },
            {
                "name": "topology_dimension",
                "type": "GINT",
                "value": "2",
                "comment": "highest dimension of a geometric element"
            },
            {
                "name": "node_coordinates",
                "value": "mesh_node_x mesh_node_y",
                "comment": "node locations in the horizontal plane"
            },
            {
                "name": "face_node_connectivity",
                "value": "mesh_face_nodes",
                "comment": "for every face, the indices of its corner nodes in a cc direction"
            },
            {
                "name": "face_dimension",
                "value": "nMeshFaces",
                "comment": "required only if non-standard; this example indicates the default, which is a face that changes slower than its nodes - i.e. (nMeshFaces, nFaceNodes) vs (nFaceNodes, nMeshFaces)"
            }
        ]
    },
    {
        "comment": "Face node coordinates: the indices of the corner nodes (in a cc direction) of each face in each mesh layer",
        "name": "mesh_face_nodes",
        "type": "GUINT",
        "args":
        [
            "nMeshFaces", 
            "nFaceNodes"
        ],
        "attributes":
        [
            {
                "name": "cf_role",
                "value": "face_node_connectivity",
                "comment": ""
            },
            {
                "name": "long_name",
                "value": "Maps every face to its corner nodes.",
                "comment": ""
            },
            {
                "name": "_FillValue",
                "type": "GUINT",
                "value": "-1",
                "comment": "type should be same as the variable type"
            },
            {
                "name": "start_index",
                "type": "GINT",
                "value": 0,
                "comment": "use 0-based indexing (i.e., points and faces are numbered starting with 0, which is the default)"
            }
        ]
    },
    {
        "comment": "Mesh node coordinates: the horizontal location (lon) of each node in each mesh layer",
        "name": "mesh_node_x",
        "type": "data_type",
        "args":
        [
            "nMeshNodes"
        ],
        "attributes":
        [
            {
                "name": "standard_name",
---EDIT---      "value": "longitude",
                "comment": ""
            },
            {
                "name": "long_name",
---EDIT---      "value": "Longitude of 2D mesh nodes.",
                "comment": ""
            },
            {
                "name": "units",
---EDIT---      "value": "degrees_east",
                "comment": ""
            }
        ]
    },
    {
        "comment": "Mesh node coordinates: the horizontal location (lat) of each node in each mesh layer",
        "name": "mesh_node_y",
        "type": "data_type",
        "args":
        [
            "nMeshNodes"
        ],
        "attributes":
        [
            {
                "name": "standard_name",
---EDIT---      "value": "latitude",
                "comment": ""
            },
            {
                "name": "long_name",
---EDIT---      "value": "Latitude of 2D mesh nodes.",
                "comment": ""
            },
            {
                "name": "units",
---EDIT---      "value": "degrees_north",
                "comment": ""
            }
        ]
    },
    {
        "comment": "Vertical coordinates: the vertical location of each mesh layer",
        "name": "meshLayers",
        "type": "data_type",
        "args":
        [
            "meshLayers"
        ],
        "attributes":
        [
            {
                "name": "units",
---EDIT---      "value": "meters",
                "comment": "required for all variables that represent dimensional quantities"
            },
            {
                "name": "positive",
---EDIT---      "value": "up",
                "comment": "“up” or “down”; direction in which coordinate values are increasing (default is \\“down\\” if the units are pressure)"
            },
            {
                "name": "standard_name",
                "value": "",
                "comment": "optional attribute - should be consistent with the positive attribute"
            },
            {
                "name": "long_name",
                "value": "",
                "comment": "optional attribute"
            },
            {
                "name": "axis",
                "value": "Z",
                "comment": "required attribute in VAPOR"
            }
        ]
    },
    {
        "comment": "Vertical coordinates: the vertical location of each mesh layer",
        "name": "mesh_depth",
        "type": "data_type",
        "args":
        [
            "meshLayers",
            "nMeshNodes"
        ],
        "attributes":
        [
            {
                "name": "units",
---EDIT---      "value": "meters",
                "comment": "required for all variables that represent dimensional quantities"
            },
            {
                "name": "positive",
---EDIT---      "value": "up",
                "comment": "“up” or “down”; direction in which coordinate values are increasing (default is \\“down\\” if the units are pressure)"
            },
            {
                "name": "standard_name",
                "value": "elevation",
                "comment": "optional attribute - should be consistent with the positive attribute"
            },
            {
                "name": "mesh",
                "value": "mesh",
                "comment": ""
            },
            {
                "name": "location",
                "value": "node",
                "comment": ""
            },
            {
                "name": "coordinates",
                "value": "mesh_node_x mesh_node_y",
                "comment": ""
            }
        ]
    },
    {
---EDIT--- "comment": "The total density field variable",
---EDIT--- "name": "dtotal",
        "type": "data_type",
        "args":
        [
            "time",
            "meshLayers",
            "nMeshNodes"
        ],
        "attributes":
        [
            {
                "name": "units",
---EDIT---      "value": "",
                "comment": "required for all variables that represent dimensional quantities"
            },
            {
                "name": "standard_name",
                "value": "",
                "comment": "optional attribute - should be consistent with the positive attribute"
            },
            {
                "name": "mesh",
                "value": "mesh",
                "comment": ""
            },
            {
                "name": "location",
                "value": "node",
                "comment": ""
            },
            {
                "name": "coordinates",
                "value": "mesh_node_x mesh_node_y mesh_depth",
                "comment": ""
            }
        ]
    },
       {
---EDIT--- "comment": "The x velocity field variable",
---EDIT--- "name": "v1",
        "type": "data_type",
        "args":
        [
            "time",
            "meshLayers",
            "nMeshNodes"
        ],
        "attributes":
        [
            {
                "name": "units",
---EDIT---      "value": "",
                "comment": "required for all variables that represent dimensional quantities"
            },
            {
                "name": "standard_name",
                "value": "",
                "comment": "optional attribute - should be consistent with the positive attribute"
            },
            {
                "name": "mesh",
                "value": "mesh",
                "comment": ""
            },
            {
                "name": "location",
                "value": "node",
                "comment": ""
            },
            {
                "name": "coordinates",
                "value": "mesh_node_x mesh_node_y mesh_depth",
                "comment": ""
            }
        ]
    }
]
}
```
