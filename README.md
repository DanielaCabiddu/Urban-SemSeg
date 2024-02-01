# Urban-SemSeg

<p align="center"><img src="teaser.png" width="1000"></p>

A pipeline to segment high-resolution point clouds representing urban environments into geometric primitives; we focus on planes, cylinders and spheres, which are the main features of buildings (walls, roofs, arches, ...) and
ground surfaces (streets, pavements, platforms), and identify the unique parameters of each instance. 

It focuses on the semantic segmentation of buildings, but the approach is currently being generalised to manage extended urban areas. Given a dense point cloud representing a specific building, we firstly apply a binary space partitioning method to obtain small enough sub-clouds that can be processed. Then, a combination of the well-known RANSAC algorithm and a recognition method based on the Hough transform (HT) is applied to each sub-cloud to obtain a semantic segmentation into salient elements, like façades, walls and roofs. The parameters of primitive instances are saved as metadata to document the structural element of buildings for further thematic analyses, e.g., energy efficiency.

## Authors
* Chiara Romanengo (CNR IMATI), Daniela Cabiddu (CNR IMATI), Simone Pittaluga (CNR IMATI), Michela Mortara (CNR IMATI)

## Citing us
Urban-SemSeg has been published [here](https://diglib.eg.org/bitstream/handle/10.2312/stag20231296/071-080.pdf).

If you use Urban-SemSeg in your academic projects, please consider citing it using the following BibTeX entries:

```bibtex
@inproceedings {10.2312:stag.20231296,
booktitle = {Smart Tools and Applications in Graphics - Eurographics Italian Chapter Conference},
editor = {Banterle, Francesco and Caggianese, Giuseppe and Capece, Nicola and Erra, Ugo and Lupinetti, Katia and Manfredi, Gilda},
title = {{Semantic Segmentation of High-resolution Point Clouds Representing Urban Contexts}},
author = {Romanengo, Chiara and Cabiddu, Daniela and Pittaluga, Simone and Mortara, Michela},
year = {2023},
publisher = {The Eurographics Association},
ISSN = {2617-4855},
ISBN = {978-3-03868-235-6},
DOI = {10.2312/stag.20231296}
}
```
