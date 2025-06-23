# Urban-SemSeg

<p align="center"><img src="teaser.png" width="1000"></p>

A pipeline to segment high-resolution point clouds representing urban environments into geometric primitives; we focus on planes, cylinders and spheres, which are the main features of buildings (walls, roofs, arches, ...) and
ground surfaces (streets, pavements, platforms), and identify the unique parameters of each instance. 

It focuses on the semantic segmentation of buildings, but the approach is currently being generalised to manage extended urban areas. Given a dense point cloud representing a specific building, we firstly apply a binary space partitioning method to obtain small enough sub-clouds that can be processed. Then, a combination of the well-known RANSAC algorithm and a recognition method based on the Hough transform (HT) is applied to each sub-cloud to obtain a semantic segmentation into salient elements, like façades, walls and roofs. The parameters of primitive instances are saved as metadata to document the structural element of buildings for further thematic analyses, e.g., energy efficiency.

## Authors
* Chiara Romanengo (CNR IMATI), Daniela Cabiddu (CNR IMATI), Simone Pittaluga (CNR IMATI), Michela Mortara (CNR IMATI)

## Citing us
If you use Urban-SemSeg in your academic projects, please consider citing it using the following BibTeX entries:

```bibtex
@article{ROMANENGO2024101234,
title = {Building semantic segmentation from large-scale point clouds via primitive recognition},
journal = {Graphical Models},
volume = {136},
pages = {101234},
year = {2024},
issn = {1524-0703},
doi = {https://doi.org/10.1016/j.gmod.2024.101234},
url = {https://www.sciencedirect.com/science/article/pii/S1524070324000225},
author = {Chiara Romanengo and Daniela Cabiddu and Simone Pittaluga and Michela Mortara},
keywords = {Point clouds, Semantic segmentation, Fitting primitives, Feature recognition, Urban digital twins},
abstract = {Modelling objects at a large resolution or scale brings challenges in the storage and processing of data and requires efficient structures. In the context of modelling urban environments, we face both issues: 3D data from acquisition extends at geographic scale, and digitization of buildings of historical value can be particularly dense. Therefore, it is crucial to exploit the point cloud derived from acquisition as much as possible, before (or alongside) deriving other representations (e.g., surface or volume meshes) for further needs (e.g., visualization, simulation). In this paper, we present our work in processing 3D data of urban areas towards the generation of a semantic model for a city digital twin. Specifically, we focus on the recognition of shape primitives (e.g., planes, cylinders, spheres) in point clouds representing urban scenes, with the main application being the semantic segmentation into walls, roofs, streets, domes, vaults, arches, and so on. Here, we extend the conference contribution in Romanengo et al. (2023a), where we presented our preliminary results on single buildings. In this extended version, we generalize the approach to manage whole cities by preliminarily splitting the point cloud building-wise and streamlining the pipeline. We added a thorough experimentation with a benchmark dataset from the city of Tallinn (47,000 buildings), a portion of Vaihingen (170 building) and our case studies in Catania and Matera, Italy (4 high-resolution buildings). Results show that our approach successfully deals with point clouds of considerable size, either surveyed at high resolution or covering wide areas. In both cases, it proves robust to input noise and outliers but sensitive to uneven sampling density.}
}
```

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
