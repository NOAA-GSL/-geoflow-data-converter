# Using NCAR's VAPOR with GeoFLOW UGRID Files

## Overview
[VAPOR](https://www.vapor.ucar.edu/) is the Visualization and Analysis Platform for Ocean, Atmospheric and Solar Researchers developed by researchers at the National Center for Atmospheric Research (NCAR). VAPOR is an interactive 3D visualization tool that allows for visualization of terascale datasets on commodity hardware using the Vapor Data Collection (VDC) data model, and it can also import other common data formats such as NetCDF, WRF and GRIB. In addition, still images and videos can be captured from the visualizations.

GeoFLOW datasets can be visualized in VAPOR by converting GeoFLOW files to the Unstructured Grid (UGRID) format and importing them into VAPOR. UGRID allows unstructured mesh data to be stored in the ubiquitous Network Common Data Form (NetCDF) file. (Note: To view GeoFLOW datasets using the more permorfant VDC data model, GeoFLOW datasets must first be interpolated into a structured grid that follow NetCDF's [CF Conventions](http://cfconventions.org/). At this time, a converter has not been written for outputting non-UGRID files).

The specification for UGRID is linked at the UGRID GitHub page [here](http://ugrid-conventions.github.io/ugrid-conventions/).

## Loading GeoFLOW Datasets

1. Open the VAPOR application.
2. From the menu, select `File > Import > UGRID`.
3. From the file browser, navigate to the GeoFLOW dataset and select the `grid.nc` file which contains the dataset's grid coordinate values. Also select the field variable `.nc` files that you are interested in loading into VAPOR. Upon selecting `Open`, a white bounding box should appear in VAPOR that indicates the extents of the dataset.
4. From the Renderers tab, select `New` and then select the renderer of choice. Choose `Slice` for 2D datasets or `Slice` or `Volume` for 3D datasets. The data should appear within the bounding box (this may take a few minutes).
5. Use the tools in the `Appearance` tab to adjust the Transfer Function to investigate the data, and use the mouse to rotate the dataset (note that holding down on the right mouse button will zoom in/out of the dataset).

## Slice Renderer Tips
- The Slice Render for UGRID files uses the GPU.
- If data is appearing with black blocky pixels when the slice is positioned at the extents of the bounding box, increase or decrease the extent value by a small amount. For example, in the Geometry tab, if the `X,Y,Z Slice Rotation` is set to `0,0,0` and the slice origin for `X,Y,Z` is set to `0,0,1`, set the `Z` value of the `Slice Origin` to `0.99`.

## Volume Renderer Tips
- VAPOR uses a third-party library, Intel's OSPRay, for the Volume Renderer for UGRID files. OSPRay is a CPU-based ray tracing rendering engine. OSPRay is threaded, so using a CPU with many cores should provide better performance.
- OSPRay is memory intensive. For example, in one test, to render 1GB of data, 32GB of RAM was required. If the system does not have enough RAM, it can start swapping heavily and might cause the system to hang.
- Sometimes the default OSPRay parameters used in the Appearance tab do not render the dataset properly. If you are barely able to see the dataset after it loads, try adjusting the `Density` parameter (which adjusts the volume density/opacity).To do this, right-click the Density text field box and change the `Maximum value` to a larger number. Then adjust the slider to the maximum value. For example, in the example box grid visualization included in this repo, the `Density` parameter had to be adjusted to `200`.
- If you are running into performance issues when using the Transfer Function or navigating through the dataset using the mouse, try adjusting the `Volume Sample Rate Scalar` parameter (which scales the sampling rate along the ray through the volume) to a lower number (for example, from `1.0` to `0.1`). This should increase the performance, however the quality of the visualization will be degraded. In this case, simply adjust the sample rate to a higher number after you have completed the transfer or navigation.
- Because VAPOR does not have control over the OSPRay rendering, viewing a UGRID volume (which again uses OSPRay and the CPU) versus viewing a slice of the UGRID volume (which uses VAPOR and the GPU) at the same viewing angle might produce slightly different results.