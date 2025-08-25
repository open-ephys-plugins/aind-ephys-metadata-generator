/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2025 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef CCFRegions_h
#define CCFRegions_h

#include <VisualizerWindowHeaders.h>

struct Region
{
    String name;
    String acronym;
    String id;
};

class CCFRegionList
{
public:
    CCFRegionList()
    {
        regions.add({"Anterior amygdalar area", "AAA", "23"});
        regions.add({"Anterior cingulate area", "ACA", "31"});
        regions.add({"Nucleus accumbens", "ACB", "56"});
        regions.add({"Accessory trigeminal nucleus", "Acs5", "549009219"});
        regions.add({"Accessory facial motor nucleus", "ACVII", "576"});
        regions.add({"Anterodorsal nucleus", "AD", "64"});
        regions.add({"Anterodorsal preoptic nucleus", "ADP", "72"});
        regions.add({"Anterior hypothalamic nucleus", "AHN", "88"});
        regions.add({"Agranular insular area", "AI", "95"});
        regions.add({"Anteromedial nucleus", "AM", "127"});
        regions.add({"Nucleus ambiguus", "AMB", "135"});
        regions.add({"Ansiform lobule", "AN", "1017"});
        regions.add({"Crus 1", "ANcr1", "1056"});
        regions.add({"Crus 2", "ANcr2", "1064"});
        regions.add({"Accessory olfactory bulb", "AOB", "151"});
        regions.add({"Anterior olfactory nucleus", "AON", "159"});
        regions.add({"Area postrema", "AP", "207"});
        regions.add({"Anterior pretectal nucleus", "APN", "215"});
        regions.add({"Area prostriata", "APr", "484682508"});
        regions.add({"Arcuate hypothalamic nucleus", "ARH", "223"});
        regions.add({"Accessory supraoptic group", "ASO", "332"});
        regions.add({"Anterior tegmental nucleus", "AT", "231"});
        regions.add({"Anterior group of the dorsal thalamus", "ATN", "239"});
        regions.add({"Auditory areas", "AUD", "247"});
        regions.add({"Dorsal auditory area", "AUDd", "1011"});
        regions.add({"Primary auditory area", "AUDp", "1002"});
        regions.add({"Posterior auditory area", "AUDpo", "1027"});
        regions.add({"Ventral auditory area", "AUDv", "1018"});
        regions.add({"Anteroventral nucleus of thalamus", "AV", "255"});
        regions.add({"Anteroventral preoptic nucleus", "AVP", "263"});
        regions.add({"Anteroventral periventricular nucleus", "AVPV", "272"});
        regions.add({"Barrington's nucleus", "B", "280"});
        regions.add({"Bed nucleus of the accessory olfactory tract", "BA", "292"});
        regions.add({"Bed nucleus of the anterior commissure", "BAC", "287"});
        regions.add({"Basolateral amygdalar nucleus", "BLA", "295"});
        regions.add({"Basomedial amygdalar nucleus", "BMA", "319"});
        regions.add({"Brain stem", "BS", "343"});
        regions.add({"Bed nuclei of the stria terminalis", "BST", "351"});
        regions.add({"Ammon's horn", "CA", "375"});
        regions.add({"Field CA1", "CA1", "382"});
        regions.add({"Field CA2", "CA2", "423"});
        regions.add({"Field CA3", "CA3", "463"});
        regions.add({"Cerebellum", "CB", "512"});
        regions.add({"Cerebellar nuclei", "CBN", "519"});
        regions.add({"Cerebellar cortex", "CBX", "528"});
        regions.add({"Central amygdalar nucleus", "CEA", "536"});
        regions.add({"Central lobule", "CENT", "920"});
        regions.add({"Lobule II", "CENT2", "976"});
        regions.add({"Lobule III", "CENT3", "984"});
        regions.add({"Cerebrum", "CH", "567"});
        regions.add({"Central lateral nucleus of the thalamus", "CL", "575"});
        regions.add({"Claustrum", "CLA", "583"});
        regions.add({"Central linear nucleus raphe", "CLI", "591"});
        regions.add({"Central medial nucleus of the thalamus", "CM", "599"});
        regions.add({"Cochlear nuclei", "CN", "607"});
        regions.add({"Cerebral nuclei", "CNU", "623"});
        regions.add({"Cortical amygdalar area", "COA", "631"});
        regions.add({"Copula pyramidis", "COPY", "1033"});
        regions.add({"Caudoputamen", "CP", "672"});
        regions.add({"Superior central nucleus raphe", "CS", "679"});
        regions.add({"Cerebral cortex", "CTX", "688"});
        regions.add({"Cortical plate", "CTXpl", "695"});
        regions.add({"Cortical subplate", "CTXsp", "703"});
        regions.add({"Cuneate nucleus", "CU", "711"});
        regions.add({"Culmen", "CUL", "928"});
        regions.add({"Lobules IV-V", "CUL4, 5", "1091"});
        regions.add({"Cuneiform nucleus", "CUN", "616"});
        regions.add({"Dorsal column nuclei", "DCN", "720"});
        regions.add({"Dorsal cochlear nucleus", "DCO", "96"});
        regions.add({"Declive (VI)", "DEC", "936"});
        regions.add({"Dentate gyrus", "DG", "726"});
        regions.add({"Dorsomedial nucleus of the hypothalamus", "DMH", "830"});
        regions.add({"Dorsal motor nucleus of the vagus nerve", "DMX", "839"});
        regions.add({"Dentate nucleus", "DN", "846"});
        regions.add({"Dorsal peduncular area", "DP", "814"});
        regions.add({"Dorsal nucleus raphe", "DR", "872"});
        regions.add({"Dorsal terminal nucleus of the accessory optic tract", "DT", "75"});
        regions.add({"Dorsal tegmental nucleus", "DTN", "880"});
        regions.add({"Ectorhinal area", "ECT", "895"});
        regions.add({"Ectorhinal area/Layer 1", "ECT1", "836"});
        regions.add({"Ectorhinal area/Layer 2/3", "ECT2/3", "427"});
        regions.add({"Ectorhinal area/Layer 5", "ECT5", "988"});
        regions.add({"Ectorhinal area/Layer 6a", "ECT6a", "977"});
        regions.add({"Ectorhinal area/Layer 6b", "ECT6b", "1045"});
        regions.add({"External cuneate nucleus", "ECU", "903"});
        regions.add({"Entorhinal area", "ENT", "909"});
        regions.add({"Endopiriform nucleus", "EP", "942"});
        regions.add({"Epithalamus", "EPI", "958"});
        regions.add({"Ethmoid nucleus of the thalamus", "Eth", "560581551"});
        regions.add({"Edinger-Westphal nucleus", "EW", "975"});
        regions.add({"Fasciola cinerea", "FC", "982"});
        regions.add({"Fields of Forel", "FF", "804"});
        regions.add({"Flocculus", "FL", "1049"});
        regions.add({"Fastigial nucleus", "FN", "989"});
        regions.add({"Folium-tuber vermis (VII)", "FOTU", "944"});
        regions.add({"Fundus of striatum", "FS", "998"});
        regions.add({"Gracile nucleus", "GR", "1039"});
        regions.add({"Basic cell groups and regions", "grey", "8"});
        regions.add({"Gigantocellular reticular nucleus", "GRN", "1048"});
        regions.add({"Gustatory areas", "GU", "1057"});
        regions.add({"Hippocampo-amygdalar transition area", "HATA", "589508447"});
        regions.add({"Hindbrain", "HB", "1065"});
        regions.add({"Hemispheric regions", "HEM", "1073"});
        regions.add({"Hippocampal region", "HIP", "1080"});
        regions.add({"Hippocampal formation", "HPF", "1089"});
        regions.add({"Hypothalamus", "HY", "1097"});
        regions.add({"Intertrigeminal nucleus", "I5", "549009227"});
        regions.add({"Intercalated amygdalar nucleus", "IA", "1105"});
        regions.add({"Interanterodorsal nucleus of the thalamus", "IAD", "1113"});
        regions.add({"Interanteromedial nucleus of the thalamus", "IAM", "1120"});
        regions.add({"Interbrain", "IB", "1129"});
        regions.add({"Inferior colliculus", "IC", "4"});
        regions.add({"Infracerebellar nucleus", "ICB", "372"});
        regions.add({"Interfascicular nucleus raphe", "IF", "12"});
        regions.add({"Induseum griseum", "IG", "19"});
        regions.add({"Intergeniculate leaflet of the lateral geniculate complex", "IGL", "27"});
        regions.add({"Oculomotor nucleus", "III", "35"});
        regions.add({"Infralimbic area", "ILA", "44"});
        regions.add({"Intralaminar nuclei of the dorsal thalamus", "ILM", "51"});
        regions.add({"Intermediodorsal nucleus of the thalamus", "IMD", "59"});
        regions.add({"Interstitial nucleus of Cajal", "INC", "67"});
        regions.add({"Intermediate geniculate nucleus", "IntG", "563807439"});
        regions.add({"Inferior olivary complex", "IO", "83"});
        regions.add({"Interposed nucleus", "IP", "91"});
        regions.add({"Interpeduncular nucleus", "IPN", "100"});
        regions.add({"Intermediate reticular nucleus", "IRN", "136"});
        regions.add({"Inferior salivatory nucleus", "ISN", "106"});
        regions.add({"Isocortex", "Isocortex", "315"});
        regions.add({"Trochlear nucleus", "IV", "115"});
        regions.add({"Koelliker-Fuse subnucleus", "KF", "123"});
        regions.add({"Lateral amygdalar nucleus", "LA", "131"});
        regions.add({"Lateral group of the dorsal thalamus", "LAT", "138"});
        regions.add({"Lateral vestibular nucleus", "LAV", "209"});
        regions.add({"Locus ceruleus", "LC", "147"});
        regions.add({"Lateral dorsal nucleus of thalamus", "LD", "155"});
        regions.add({"Laterodorsal tegmental nucleus", "LDT", "162"});
        regions.add({"Dorsal part of the lateral geniculate complex", "LGd", "170"});
        regions.add({"Ventral part of the lateral geniculate complex", "LGv", "178"});
        regions.add({"Lateral habenula", "LH", "186"});
        regions.add({"Lateral hypothalamic area", "LHA", "194"});
        regions.add({"Linear nucleus of the medulla", "LIN", "203"});
        regions.add({"Lingula (I)", "LING", "912"});
        regions.add({"Lateral mammillary nucleus", "LM", "210"});
        regions.add({"Lateral posterior nucleus of the thalamus", "LP", "218"});
        regions.add({"Lateral preoptic area", "LPO", "226"});
        regions.add({"Lateral reticular nucleus", "LRN", "235"});
        regions.add({"Lateral septal nucleus", "LS", "242"});
        regions.add({"Lateral septal complex", "LSX", "275"});
        regions.add({"Lateral terminal nucleus of the accessory optic tract", "LT", "66"});
        regions.add({"Hypothalamic lateral zone", "LZ", "290"});
        regions.add({"Magnocellular nucleus", "MA", "298"});
        regions.add({"Medial accesory oculomotor nucleus", "MA3", "549009211"});
        regions.add({"Magnocellular reticular nucleus", "MARN", "307"});
        regions.add({"Midbrain", "MB", "313"});
        regions.add({"Mammillary body", "MBO", "331"});
        regions.add({"Mediodorsal nucleus of thalamus", "MD", "362"});
        regions.add({"Medullary reticular nucleus", "MDRN", "395"});
        regions.add({"Median eminence", "ME", "10671"});
        regions.add({"Medial amygdalar nucleus", "MEA", "403"});
        regions.add({"Medial group of the dorsal thalamus", "MED", "444"});
        regions.add({"Median preoptic nucleus", "MEPO", "452"});
        regions.add({"Midbrain trigeminal nucleus", "MEV", "460"});
        regions.add({"Hypothalamic medial zone", "MEZ", "467"});
        regions.add({"Medial geniculate complex", "MG", "475"});
        regions.add({"Medial habenula", "MH", "483"});
        regions.add({"Medial mammillary nucleus", "MM", "491"});
        regions.add({"Somatomotor areas", "MO", "500"});
        regions.add({"Main olfactory bulb", "MOB", "507"});
        regions.add({"Primary motor area", "MOp", "985"});
        regions.add({"Secondary motor area", "MOs", "993"});
        regions.add({"Medial preoptic nucleus", "MPN", "515"});
        regions.add({"Medial preoptic area", "MPO", "523"});
        regions.add({"Medial pretectal area", "MPT", "531"});
        regions.add({"Midbrain reticular nucleus", "MRN", "128"});
        regions.add({"Medial septal nucleus", "MS", "564"});
        regions.add({"Medial septal complex", "MSC", "904"});
        regions.add({"Medial terminal nucleus of the accessory optic tract", "MT", "58"});
        regions.add({"Midline group of the dorsal thalamus", "MTN", "571"});
        regions.add({"Medial vestibular nucleus", "MV", "202"});
        regions.add({"Medulla", "MY", "354"});
        regions.add({"Nucleus of the brachium of the inferior colliculus", "NB", "580"});
        regions.add({"Nucleus of Darkschewitsch", "ND", "587"});
        regions.add({"Diagonal band nucleus", "NDB", "596"});
        regions.add({"Nucleus incertus", "NI", "604"});
        regions.add({"Nucleus of the lateral lemniscus", "NLL", "612"});
        regions.add({"Nucleus of the lateral olfactory tract", "NLOT", "619"});
        regions.add({"Nodulus (X)", "NOD", "968"});
        regions.add({"Nucleus of the optic tract", "NOT", "628"});
        regions.add({"Nucleus of the posterior commissure", "NPC", "634"});
        regions.add({"Nucleus of Roller", "NR", "177"});
        regions.add({"Nucleus of the trapezoid body", "NTB", "642"});
        regions.add({"Nucleus of the solitary tract", "NTS", "651"});
        regions.add({"Olfactory areas", "OLF", "698"});
        regions.add({"Olivary pretectal nucleus", "OP", "706"});
        regions.add({"Orbital area", "ORB", "714"});
        regions.add({"Olfactory tubercle", "OT", "754"});
        regions.add({"Vascular organ of the lamina terminalis", "OV", "763"});
        regions.add({"Pons", "P", "771"});
        regions.add({"Peritrigeminal zone", "P5", "549009215"});
        regions.add({"Posterior amygdalar nucleus", "PA", "780"});
        regions.add({"Paratrochlear nucleus", "Pa4", "606826663"});
        regions.add({"Paratrigeminal nucleus", "Pa5", "589508451"});
        regions.add({"Piriform-amygdalar area", "PAA", "788"});
        regions.add({"Periaqueductal gray", "PAG", "795"});
        regions.add({"Pallidum", "PAL", "803"});
        regions.add({"Parasubiculum", "PAR", "843"});
        regions.add({"Parvicellular reticular nucleus", "PARN", "852"});
        regions.add({"Parasolitary nucleus", "PAS", "859"});
        regions.add({"Parabrachial nucleus", "PB", "867"});
        regions.add({"Parabigeminal nucleus", "PBG", "874"});
        regions.add({"Parvicellular motor 5 nucleus", "PC5", "549009223"});
        regions.add({"Pontine central gray", "PCG", "898"});
        regions.add({"Paracentral nucleus", "PCN", "907"});
        regions.add({"Posterodorsal preoptic nucleus", "PD", "914"});
        regions.add({"Posterodorsal tegmental nucleus", "PDTg", "599626927"});
        regions.add({"Perifornical nucleus", "PeF", "576073704"});
        regions.add({"Perirhinal area", "PERI", "922"});
        regions.add({"Parafascicular nucleus", "PF", "930"});
        regions.add({"Paraflocculus", "PFL", "1041"});
        regions.add({"Pontine gray", "PG", "931"});
        regions.add({"Paragigantocellular reticular nucleus", "PGRN", "938"});
        regions.add({"Posterior hypothalamic nucleus", "PH", "946"});
        regions.add({"Perihypoglossal nuclei", "PHY", "154"});
        regions.add({"Posterior intralaminar thalamic nucleus", "PIL", "560581563"});
        regions.add({"Piriform area", "PIR", "961"});
        regions.add({"Prelimbic area", "PL", "972"});
        regions.add({"Dorsal premammillary nucleus", "PMd", "980"});
        regions.add({"Ventral premammillary nucleus", "PMv", "1004"});
        regions.add({"Paranigral nucleus", "PN", "607344830"});
        regions.add({"Posterior complex of the thalamus", "PO", "1020"});
        regions.add({"Posterior limiting nucleus of the thalamus", "POL", "1029"});
        regions.add({"Postsubiculum", "POST", "1037"});
        regions.add({"Posterior triangular thalamic nucleus", "PoT", "563807435"});
        regions.add({"Peripeduncular nucleus", "PP", "1044"});
        regions.add({"Pedunculopontine nucleus", "PPN", "1052"});
        regions.add({"Posterior pretectal nucleus", "PPT", "1061"});
        regions.add({"Parapyramidal nucleus", "PPY", "1069"});
        regions.add({"Perireunensis nucleus", "PR", "1077"});
        regions.add({"Precommissural nucleus", "PRC", "50"});
        regions.add({"Presubiculum", "PRE", "1084"});
        regions.add({"Paramedian lobule", "PRM", "1025"});
        regions.add({"Pontine reticular nucleus", "PRNr", "146"});
        regions.add({"Prosubiculum", "ProS", "484682470"});
        regions.add({"Nucleus prepositus", "PRP", "169"});
        regions.add({"Pretectal region", "PRT", "1100"});
        regions.add({"Parastrial nucleus", "PS", "1109"});
        regions.add({"Preparasubthalamic nucleus", "PST", "356"});
        regions.add({"Parasubthalamic nucleus", "PSTN", "364"});
        regions.add({"Principal sensory nucleus of the trigeminal", "PSV", "7"});
        regions.add({"Parataenial nucleus", "PT", "15"});
        regions.add({"Posterior parietal association areas", "PTLp", "22"});
        regions.add({"Paraventricular hypothalamic nucleus", "PVH", "38"});
        regions.add({"Periventricular region", "PVR", "141"});
        regions.add({"Paraventricular nucleus of the thalamus", "PVT", "149"});
        regions.add({"Periventricular zone", "PVZ", "157"});
        regions.add({"Pyramus (VIII)", "PYR", "951"});
        regions.add({"Midbrain raphe nuclei", "RAmb", "165"});
        regions.add({"Retrochiasmatic area", "RCH", "173"});
        regions.add({"Nucleus of reuniens", "RE", "181"});
        regions.add({"Rhomboid nucleus", "RH", "189"});
        regions.add({"Retrohippocampal region", "RHP", "822"});
        regions.add({"Rostral linear nucleus raphe", "RL", "197"});
        regions.add({"Nucleus raphe magnus", "RM", "206"});
        regions.add({"Red nucleus", "RN", "214"});
        regions.add({"Nucleus raphe obscurus", "RO", "222"});
        regions.add({"Nucleus raphe pallidus", "RPA", "230"});
        regions.add({"Retroparafascicular nucleus", "RPF", "549009203"});
        regions.add({"Nucleus raphe pontis", "RPO", "238"});
        regions.add({"Retrosplenial area", "RSP", "254"});
        regions.add({"Reticular nucleus of the thalamus", "RT", "262"});
        regions.add({"Nucleus sagulum", "SAG", "271"});
        regions.add({"Striatum-like amygdalar nuclei", "sAMY", "278"});
        regions.add({"Subparaventricular zone", "SBPV", "347"});
        regions.add({"Suprachiasmatic nucleus", "SCH", "286"});
        regions.add({"Subcommissural organ", "SCO", "599626923"});
        regions.add({"Septofimbrial nucleus", "SF", "310"});
        regions.add({"Subfornical organ", "SFO", "338"});
        regions.add({"Supragenual nucleus", "SG", "318"});
        regions.add({"Suprageniculate nucleus", "SGN", "325"});
        regions.add({"Septohippocampal nucleus", "SH", "333"});
        regions.add({"Substantia innominata", "SI", "342"});
        regions.add({"Simple lobule", "SIM", "1007"});
        regions.add({"Subceruleus nucleus", "SLC", "350"});
        regions.add({"Sublaterodorsal nucleus", "SLD", "358"});
        regions.add({"Submedial nucleus of the thalamus", "SMT", "366"});
        regions.add({"Supraoptic nucleus", "SO", "390"});
        regions.add({"Superior olivary complex", "SOC", "398"});
        regions.add({"Subparafascicular area", "SPA", "609"});
        regions.add({"Subparafascicular nucleus", "SPF", "406"});
        regions.add({"Spinal vestibular nucleus", "SPIV", "225"});
        regions.add({"Somatosensory areas", "SS", "453"});
        regions.add({"Primary somatosensory area", "SSp", "322"});
        regions.add({"Supplemental somatosensory area", "SSs", "378"});
        regions.add({"Subthalamic nucleus", "STN", "470"});
        regions.add({"Striatum", "STR", "477"});
        regions.add({"Striatum dorsal region", "STRd", "485"});
        regions.add({"Striatum ventral region", "STRv", "493"});
        regions.add({"Supraoculomotor periaqueductal gray", "Su3", "614454277"});
        regions.add({"Subiculum", "SUB", "502"});
        regions.add({"Subgeniculate nucleus", "SubG", "321"});
        regions.add({"Supramammillary nucleus", "SUM", "525"});
        regions.add({"Supratrigeminal nucleus", "SUT", "534"});
        regions.add({"Superior vestibular nucleus", "SUV", "217"});
        regions.add({"Temporal association areas", "TEa", "541"});
        regions.add({"Thalamus", "TH", "549"});
        regions.add({"Tuberomammillary nucleus", "TM", "557"});
        regions.add({"Postpiriform transition area", "TR", "566"});
        regions.add({"Tegmental reticular nucleus", "TRN", "574"});
        regions.add({"Triangular nucleus of septum", "TRS", "581"});
        regions.add({"Taenia tecta", "TT", "589"});
        regions.add({"Tuberal nucleus", "TU", "614"});
        regions.add({"Uvula (IX)", "UVU", "957"});
        regions.add({"Motor nucleus of trigeminal", "V", "621"});
        regions.add({"Ventral anterior-lateral complex of the thalamus", "VAL", "629"});
        regions.add({"Ventral cochlear nucleus", "VCO", "101"});
        regions.add({"Vestibulocerebellar nucleus", "VeCB", "589508455"});
        regions.add({"Ventral group of the dorsal thalamus", "VENT", "637"});
        regions.add({"Vermal regions", "VERM", "645"});
        regions.add({"Abducens nucleus", "VI", "653"});
        regions.add({"Facial motor nucleus", "VII", "661"});
        regions.add({"Visual areas", "VIS", "669"});
        regions.add({"Anterior area", "VISa", "312782546"});
        regions.add({"Anterolateral visual area", "VISal", "402"});
        regions.add({"Anteromedial visual area", "VISam", "394"});
        regions.add({"Visceral area", "VISC", "677"});
        regions.add({"Lateral visual area", "VISl", "409"});
        regions.add({"Laterointermediate area", "VISli", "312782574"});
        regions.add({"Primary visual area", "VISp", "385"});
        regions.add({"Posterolateral visual area", "VISpl", "425"});
        regions.add({"Postrhinal area", "VISpor", "312782628"});
        regions.add({"Rostrolateral visual area", "VISrl", "417"});
        regions.add({"Ventrolateral preoptic nucleus", "VLPO", "689"});
        regions.add({"Ventral medial nucleus of the thalamus", "VM", "685"});
        regions.add({"Ventromedial hypothalamic nucleus", "VMH", "693"});
        regions.add({"Ventromedial preoptic nucleus", "VMPO", "576073699"});
        regions.add({"Vestibular nuclei", "VNC", "701"});
        regions.add({"Ventral posterior complex of the thalamus", "VP", "709"});
        regions.add({"Ventral posterolateral nucleus of the thalamus", "VPL", "718"});
        regions.add({"Ventral posteromedial nucleus of the thalamus", "VPM", "733"});
        regions.add({"Ventral tegmental area", "VTA", "749"});
        regions.add({"Ventral tegmental nucleus", "VTN", "757"});
        regions.add({"Nucleus x", "x", "765"});
        regions.add({"Xiphoid thalamic nucleus", "Xi", "560581559"});
        regions.add({"Hypoglossal nucleus", "XII", "773"});
        regions.add({"Nucleus y", "y", "781"});
        regions.add({"Zona incerta", "ZI", "797"});
    }
    
    ~CCFRegionList() { }
    
    /** Returns the list of regions */
    StringArray getRegionAcronyms() const
    {
        StringArray names;
        for (const auto& region : regions)
        {
            names.add(region.acronym);
        }
        return names;
    }
    
    /** 
     * Finds a region by its acronym or name
     * @param identifier The acronym or name to search for
     * @param searchByName If true, searches by name; if false, searches by acronym (default)
     * @return Region struct if found, empty Region with "Unknown" values if not found
     */
    Region findRegion(const String& identifier, bool searchByName = false) const
    {
        if (identifier.isEmpty())
        {
            // Return empty region
            return Region { "root", "root", "997" };
        }
        
        for (const auto& region : regions)
        {
            if (searchByName)
            {
                if (region.name.equalsIgnoreCase(identifier))
                {
                    return region;
                }
            }
            else
            {
                if (region.acronym.equalsIgnoreCase(identifier))
                {
                    return region;
                }
            }
        }
        
        // If not found, return a default "root" region
        return Region { "root", "root", "997" };
    }
    
    /**
     * Finds a region by acronym (convenience method)
     * @param acronym The acronym to search for
     * @return Region struct if found, empty Region with "Unknown" values if not found
     */
    Region findRegionByAcronym(const String& acronym) const
    {
        return findRegion(acronym, false);
    }
    
    /**
     * Finds a region by name (convenience method)
     * @param name The name to search for
     * @return Region struct if found, empty Region with "Unknown" values if not found
     */
    Region findRegionByName(const String& name) const
    {
        return findRegion(name, true);
    }
    
    Array<Region> regions;
};

#endif /* CCFRegions_h */
