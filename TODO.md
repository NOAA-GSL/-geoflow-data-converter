
# GeoFLOW Data Converter TO DO List

## TO DO
- Add error handling to NetCDF calls (g_to_netcdf.h/cpp).
- Add custom enum in place of second param for NcFile mode to remove Nc  
dependency?
- g_to_netcdf::writeVariableData is allocating all node data but this is not  
needed for all vars since, for ex, mesh_nodes_x is the same for each layer;  
it is getting correctly written to the NetCDF file, I'm guessing b/c of the  
dimensions that were alreay assigned to the variable; Writing of the single  
variable seems hacky - update.
- Is custom g_to_netcdf::putAttribute() method needed?
- Check attribute values: some ints are set as strings and then converted?
- See Fill Value for mesh_face_nodes (what should it's type be?)

## TO OPTIMIZE
- Don't need to read the element layer IDs for all x,y,z grid files as they  
are the same, just need one.
- 2D mesh layers that reside between GeoFLOW element layers are getting  
written twice. Duplicate layers should be eliminated for space and performance.
