/*******************************************************************************
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "crc64.h"

static const uint64_t crc64_ecma_refl_table[256] = {
	0x0000000000000000ULL, 0xb32e4cbe03a75f6fULL,
	0xf4843657a840a05bULL, 0x47aa7ae9abe7ff34ULL,
	0x7bd0c384ff8f5e33ULL, 0xc8fe8f3afc28015cULL,
	0x8f54f5d357cffe68ULL, 0x3c7ab96d5468a107ULL,
	0xf7a18709ff1ebc66ULL, 0x448fcbb7fcb9e309ULL,
	0x0325b15e575e1c3dULL, 0xb00bfde054f94352ULL,
	0x8c71448d0091e255ULL, 0x3f5f08330336bd3aULL,
	0x78f572daa8d1420eULL, 0xcbdb3e64ab761d61ULL,
	0x7d9ba13851336649ULL, 0xceb5ed8652943926ULL,
	0x891f976ff973c612ULL, 0x3a31dbd1fad4997dULL,
	0x064b62bcaebc387aULL, 0xb5652e02ad1b6715ULL,
	0xf2cf54eb06fc9821ULL, 0x41e11855055bc74eULL,
	0x8a3a2631ae2dda2fULL, 0x39146a8fad8a8540ULL,
	0x7ebe1066066d7a74ULL, 0xcd905cd805ca251bULL,
	0xf1eae5b551a2841cULL, 0x42c4a90b5205db73ULL,
	0x056ed3e2f9e22447ULL, 0xb6409f5cfa457b28ULL,
	0xfb374270a266cc92ULL, 0x48190ecea1c193fdULL,
	0x0fb374270a266cc9ULL, 0xbc9d3899098133a6ULL,
	0x80e781f45de992a1ULL, 0x33c9cd4a5e4ecdceULL,
	0x7463b7a3f5a932faULL, 0xc74dfb1df60e6d95ULL,
	0x0c96c5795d7870f4ULL, 0xbfb889c75edf2f9bULL,
	0xf812f32ef538d0afULL, 0x4b3cbf90f69f8fc0ULL,
	0x774606fda2f72ec7ULL, 0xc4684a43a15071a8ULL,
	0x83c230aa0ab78e9cULL, 0x30ec7c140910d1f3ULL,
	0x86ace348f355aadbULL, 0x3582aff6f0f2f5b4ULL,
	0x7228d51f5b150a80ULL, 0xc10699a158b255efULL,
	0xfd7c20cc0cdaf4e8ULL, 0x4e526c720f7dab87ULL,
	0x09f8169ba49a54b3ULL, 0xbad65a25a73d0bdcULL,
	0x710d64410c4b16bdULL, 0xc22328ff0fec49d2ULL,
	0x85895216a40bb6e6ULL, 0x36a71ea8a7ace989ULL,
	0x0adda7c5f3c4488eULL, 0xb9f3eb7bf06317e1ULL,
	0xfe5991925b84e8d5ULL, 0x4d77dd2c5823b7baULL,
	0x64b62bcaebc387a1ULL, 0xd7986774e864d8ceULL,
	0x90321d9d438327faULL, 0x231c512340247895ULL,
	0x1f66e84e144cd992ULL, 0xac48a4f017eb86fdULL,
	0xebe2de19bc0c79c9ULL, 0x58cc92a7bfab26a6ULL,
	0x9317acc314dd3bc7ULL, 0x2039e07d177a64a8ULL,
	0x67939a94bc9d9b9cULL, 0xd4bdd62abf3ac4f3ULL,
	0xe8c76f47eb5265f4ULL, 0x5be923f9e8f53a9bULL,
	0x1c4359104312c5afULL, 0xaf6d15ae40b59ac0ULL,
	0x192d8af2baf0e1e8ULL, 0xaa03c64cb957be87ULL,
	0xeda9bca512b041b3ULL, 0x5e87f01b11171edcULL,
	0x62fd4976457fbfdbULL, 0xd1d305c846d8e0b4ULL,
	0x96797f21ed3f1f80ULL, 0x2557339fee9840efULL,
	0xee8c0dfb45ee5d8eULL, 0x5da24145464902e1ULL,
	0x1a083bacedaefdd5ULL, 0xa9267712ee09a2baULL,
	0x955cce7fba6103bdULL, 0x267282c1b9c65cd2ULL,
	0x61d8f8281221a3e6ULL, 0xd2f6b4961186fc89ULL,
	0x9f8169ba49a54b33ULL, 0x2caf25044a02145cULL,
	0x6b055fede1e5eb68ULL, 0xd82b1353e242b407ULL,
	0xe451aa3eb62a1500ULL, 0x577fe680b58d4a6fULL,
	0x10d59c691e6ab55bULL, 0xa3fbd0d71dcdea34ULL,
	0x6820eeb3b6bbf755ULL, 0xdb0ea20db51ca83aULL,
	0x9ca4d8e41efb570eULL, 0x2f8a945a1d5c0861ULL,
	0x13f02d374934a966ULL, 0xa0de61894a93f609ULL,
	0xe7741b60e174093dULL, 0x545a57dee2d35652ULL,
	0xe21ac88218962d7aULL, 0x5134843c1b317215ULL,
	0x169efed5b0d68d21ULL, 0xa5b0b26bb371d24eULL,
	0x99ca0b06e7197349ULL, 0x2ae447b8e4be2c26ULL,
	0x6d4e3d514f59d312ULL, 0xde6071ef4cfe8c7dULL,
	0x15bb4f8be788911cULL, 0xa6950335e42fce73ULL,
	0xe13f79dc4fc83147ULL, 0x521135624c6f6e28ULL,
	0x6e6b8c0f1807cf2fULL, 0xdd45c0b11ba09040ULL,
	0x9aefba58b0476f74ULL, 0x29c1f6e6b3e0301bULL,
	0xc96c5795d7870f42ULL, 0x7a421b2bd420502dULL,
	0x3de861c27fc7af19ULL, 0x8ec62d7c7c60f076ULL,
	0xb2bc941128085171ULL, 0x0192d8af2baf0e1eULL,
	0x4638a2468048f12aULL, 0xf516eef883efae45ULL,
	0x3ecdd09c2899b324ULL, 0x8de39c222b3eec4bULL,
	0xca49e6cb80d9137fULL, 0x7967aa75837e4c10ULL,
	0x451d1318d716ed17ULL, 0xf6335fa6d4b1b278ULL,
	0xb199254f7f564d4cULL, 0x02b769f17cf11223ULL,
	0xb4f7f6ad86b4690bULL, 0x07d9ba1385133664ULL,
	0x4073c0fa2ef4c950ULL, 0xf35d8c442d53963fULL,
	0xcf273529793b3738ULL, 0x7c0979977a9c6857ULL,
	0x3ba3037ed17b9763ULL, 0x888d4fc0d2dcc80cULL,
	0x435671a479aad56dULL, 0xf0783d1a7a0d8a02ULL,
	0xb7d247f3d1ea7536ULL, 0x04fc0b4dd24d2a59ULL,
	0x3886b22086258b5eULL, 0x8ba8fe9e8582d431ULL,
	0xcc0284772e652b05ULL, 0x7f2cc8c92dc2746aULL,
	0x325b15e575e1c3d0ULL, 0x8175595b76469cbfULL,
	0xc6df23b2dda1638bULL, 0x75f16f0cde063ce4ULL,
	0x498bd6618a6e9de3ULL, 0xfaa59adf89c9c28cULL,
	0xbd0fe036222e3db8ULL, 0x0e21ac88218962d7ULL,
	0xc5fa92ec8aff7fb6ULL, 0x76d4de52895820d9ULL,
	0x317ea4bb22bfdfedULL, 0x8250e80521188082ULL,
	0xbe2a516875702185ULL, 0x0d041dd676d77eeaULL,
	0x4aae673fdd3081deULL, 0xf9802b81de97deb1ULL,
	0x4fc0b4dd24d2a599ULL, 0xfceef8632775faf6ULL,
	0xbb44828a8c9205c2ULL, 0x086ace348f355aadULL,
	0x34107759db5dfbaaULL, 0x873e3be7d8faa4c5ULL,
	0xc094410e731d5bf1ULL, 0x73ba0db070ba049eULL,
	0xb86133d4dbcc19ffULL, 0x0b4f7f6ad86b4690ULL,
	0x4ce50583738cb9a4ULL, 0xffcb493d702be6cbULL,
	0xc3b1f050244347ccULL, 0x709fbcee27e418a3ULL,
	0x3735c6078c03e797ULL, 0x841b8ab98fa4b8f8ULL,
	0xadda7c5f3c4488e3ULL, 0x1ef430e13fe3d78cULL,
	0x595e4a08940428b8ULL, 0xea7006b697a377d7ULL,
	0xd60abfdbc3cbd6d0ULL, 0x6524f365c06c89bfULL,
	0x228e898c6b8b768bULL, 0x91a0c532682c29e4ULL,
	0x5a7bfb56c35a3485ULL, 0xe955b7e8c0fd6beaULL,
	0xaeffcd016b1a94deULL, 0x1dd181bf68bdcbb1ULL,
	0x21ab38d23cd56ab6ULL, 0x9285746c3f7235d9ULL,
	0xd52f0e859495caedULL, 0x6601423b97329582ULL,
	0xd041dd676d77eeaaULL, 0x636f91d96ed0b1c5ULL,
	0x24c5eb30c5374ef1ULL, 0x97eba78ec690119eULL,
	0xab911ee392f8b099ULL, 0x18bf525d915feff6ULL,
	0x5f1528b43ab810c2ULL, 0xec3b640a391f4fadULL,
	0x27e05a6e926952ccULL, 0x94ce16d091ce0da3ULL,
	0xd3646c393a29f297ULL, 0x604a2087398eadf8ULL,
	0x5c3099ea6de60cffULL, 0xef1ed5546e415390ULL,
	0xa8b4afbdc5a6aca4ULL, 0x1b9ae303c601f3cbULL,
	0x56ed3e2f9e224471ULL, 0xe5c372919d851b1eULL,
	0xa26908783662e42aULL, 0x114744c635c5bb45ULL,
	0x2d3dfdab61ad1a42ULL, 0x9e13b115620a452dULL,
	0xd9b9cbfcc9edba19ULL, 0x6a978742ca4ae576ULL,
	0xa14cb926613cf817ULL, 0x1262f598629ba778ULL,
	0x55c88f71c97c584cULL, 0xe6e6c3cfcadb0723ULL,
	0xda9c7aa29eb3a624ULL, 0x69b2361c9d14f94bULL,
	0x2e184cf536f3067fULL, 0x9d36004b35545910ULL,
	0x2b769f17cf112238ULL, 0x9858d3a9ccb67d57ULL,
	0xdff2a94067518263ULL, 0x6cdce5fe64f6dd0cULL,
	0x50a65c93309e7c0bULL, 0xe388102d33392364ULL,
	0xa4226ac498dedc50ULL, 0x170c267a9b79833fULL,
	0xdcd7181e300f9e5eULL, 0x6ff954a033a8c131ULL,
	0x28532e49984f3e05ULL, 0x9b7d62f79be8616aULL,
	0xa707db9acf80c06dULL, 0x14299724cc279f02ULL,
	0x5383edcd67c06036ULL, 0xe0ada17364673f59ULL
};

static const uint64_t crc64_ecma_norm_table[256] = {
	0x0000000000000000ULL, 0x42f0e1eba9ea3693ULL,
	0x85e1c3d753d46d26ULL, 0xc711223cfa3e5bb5ULL,
	0x493366450e42ecdfULL, 0x0bc387aea7a8da4cULL,
	0xccd2a5925d9681f9ULL, 0x8e224479f47cb76aULL,
	0x9266cc8a1c85d9beULL, 0xd0962d61b56fef2dULL,
	0x17870f5d4f51b498ULL, 0x5577eeb6e6bb820bULL,
	0xdb55aacf12c73561ULL, 0x99a54b24bb2d03f2ULL,
	0x5eb4691841135847ULL, 0x1c4488f3e8f96ed4ULL,
	0x663d78ff90e185efULL, 0x24cd9914390bb37cULL,
	0xe3dcbb28c335e8c9ULL, 0xa12c5ac36adfde5aULL,
	0x2f0e1eba9ea36930ULL, 0x6dfeff5137495fa3ULL,
	0xaaefdd6dcd770416ULL, 0xe81f3c86649d3285ULL,
	0xf45bb4758c645c51ULL, 0xb6ab559e258e6ac2ULL,
	0x71ba77a2dfb03177ULL, 0x334a9649765a07e4ULL,
	0xbd68d2308226b08eULL, 0xff9833db2bcc861dULL,
	0x388911e7d1f2dda8ULL, 0x7a79f00c7818eb3bULL,
	0xcc7af1ff21c30bdeULL, 0x8e8a101488293d4dULL,
	0x499b3228721766f8ULL, 0x0b6bd3c3dbfd506bULL,
	0x854997ba2f81e701ULL, 0xc7b97651866bd192ULL,
	0x00a8546d7c558a27ULL, 0x4258b586d5bfbcb4ULL,
	0x5e1c3d753d46d260ULL, 0x1cecdc9e94ace4f3ULL,
	0xdbfdfea26e92bf46ULL, 0x990d1f49c77889d5ULL,
	0x172f5b3033043ebfULL, 0x55dfbadb9aee082cULL,
	0x92ce98e760d05399ULL, 0xd03e790cc93a650aULL,
	0xaa478900b1228e31ULL, 0xe8b768eb18c8b8a2ULL,
	0x2fa64ad7e2f6e317ULL, 0x6d56ab3c4b1cd584ULL,
	0xe374ef45bf6062eeULL, 0xa1840eae168a547dULL,
	0x66952c92ecb40fc8ULL, 0x2465cd79455e395bULL,
	0x3821458aada7578fULL, 0x7ad1a461044d611cULL,
	0xbdc0865dfe733aa9ULL, 0xff3067b657990c3aULL,
	0x711223cfa3e5bb50ULL, 0x33e2c2240a0f8dc3ULL,
	0xf4f3e018f031d676ULL, 0xb60301f359dbe0e5ULL,
	0xda050215ea6c212fULL, 0x98f5e3fe438617bcULL,
	0x5fe4c1c2b9b84c09ULL, 0x1d14202910527a9aULL,
	0x93366450e42ecdf0ULL, 0xd1c685bb4dc4fb63ULL,
	0x16d7a787b7faa0d6ULL, 0x5427466c1e109645ULL,
	0x4863ce9ff6e9f891ULL, 0x0a932f745f03ce02ULL,
	0xcd820d48a53d95b7ULL, 0x8f72eca30cd7a324ULL,
	0x0150a8daf8ab144eULL, 0x43a04931514122ddULL,
	0x84b16b0dab7f7968ULL, 0xc6418ae602954ffbULL,
	0xbc387aea7a8da4c0ULL, 0xfec89b01d3679253ULL,
	0x39d9b93d2959c9e6ULL, 0x7b2958d680b3ff75ULL,
	0xf50b1caf74cf481fULL, 0xb7fbfd44dd257e8cULL,
	0x70eadf78271b2539ULL, 0x321a3e938ef113aaULL,
	0x2e5eb66066087d7eULL, 0x6cae578bcfe24bedULL,
	0xabbf75b735dc1058ULL, 0xe94f945c9c3626cbULL,
	0x676dd025684a91a1ULL, 0x259d31cec1a0a732ULL,
	0xe28c13f23b9efc87ULL, 0xa07cf2199274ca14ULL,
	0x167ff3eacbaf2af1ULL, 0x548f120162451c62ULL,
	0x939e303d987b47d7ULL, 0xd16ed1d631917144ULL,
	0x5f4c95afc5edc62eULL, 0x1dbc74446c07f0bdULL,
	0xdaad56789639ab08ULL, 0x985db7933fd39d9bULL,
	0x84193f60d72af34fULL, 0xc6e9de8b7ec0c5dcULL,
	0x01f8fcb784fe9e69ULL, 0x43081d5c2d14a8faULL,
	0xcd2a5925d9681f90ULL, 0x8fdab8ce70822903ULL,
	0x48cb9af28abc72b6ULL, 0x0a3b7b1923564425ULL,
	0x70428b155b4eaf1eULL, 0x32b26afef2a4998dULL,
	0xf5a348c2089ac238ULL, 0xb753a929a170f4abULL,
	0x3971ed50550c43c1ULL, 0x7b810cbbfce67552ULL,
	0xbc902e8706d82ee7ULL, 0xfe60cf6caf321874ULL,
	0xe224479f47cb76a0ULL, 0xa0d4a674ee214033ULL,
	0x67c58448141f1b86ULL, 0x253565a3bdf52d15ULL,
	0xab1721da49899a7fULL, 0xe9e7c031e063acecULL,
	0x2ef6e20d1a5df759ULL, 0x6c0603e6b3b7c1caULL,
	0xf6fae5c07d3274cdULL, 0xb40a042bd4d8425eULL,
	0x731b26172ee619ebULL, 0x31ebc7fc870c2f78ULL,
	0xbfc9838573709812ULL, 0xfd39626eda9aae81ULL,
	0x3a28405220a4f534ULL, 0x78d8a1b9894ec3a7ULL,
	0x649c294a61b7ad73ULL, 0x266cc8a1c85d9be0ULL,
	0xe17dea9d3263c055ULL, 0xa38d0b769b89f6c6ULL,
	0x2daf4f0f6ff541acULL, 0x6f5faee4c61f773fULL,
	0xa84e8cd83c212c8aULL, 0xeabe6d3395cb1a19ULL,
	0x90c79d3fedd3f122ULL, 0xd2377cd44439c7b1ULL,
	0x15265ee8be079c04ULL, 0x57d6bf0317edaa97ULL,
	0xd9f4fb7ae3911dfdULL, 0x9b041a914a7b2b6eULL,
	0x5c1538adb04570dbULL, 0x1ee5d94619af4648ULL,
	0x02a151b5f156289cULL, 0x4051b05e58bc1e0fULL,
	0x87409262a28245baULL, 0xc5b073890b687329ULL,
	0x4b9237f0ff14c443ULL, 0x0962d61b56fef2d0ULL,
	0xce73f427acc0a965ULL, 0x8c8315cc052a9ff6ULL,
	0x3a80143f5cf17f13ULL, 0x7870f5d4f51b4980ULL,
	0xbf61d7e80f251235ULL, 0xfd913603a6cf24a6ULL,
	0x73b3727a52b393ccULL, 0x31439391fb59a55fULL,
	0xf652b1ad0167feeaULL, 0xb4a25046a88dc879ULL,
	0xa8e6d8b54074a6adULL, 0xea16395ee99e903eULL,
	0x2d071b6213a0cb8bULL, 0x6ff7fa89ba4afd18ULL,
	0xe1d5bef04e364a72ULL, 0xa3255f1be7dc7ce1ULL,
	0x64347d271de22754ULL, 0x26c49cccb40811c7ULL,
	0x5cbd6cc0cc10fafcULL, 0x1e4d8d2b65facc6fULL,
	0xd95caf179fc497daULL, 0x9bac4efc362ea149ULL,
	0x158e0a85c2521623ULL, 0x577eeb6e6bb820b0ULL,
	0x906fc95291867b05ULL, 0xd29f28b9386c4d96ULL,
	0xcedba04ad0952342ULL, 0x8c2b41a1797f15d1ULL,
	0x4b3a639d83414e64ULL, 0x09ca82762aab78f7ULL,
	0x87e8c60fded7cf9dULL, 0xc51827e4773df90eULL,
	0x020905d88d03a2bbULL, 0x40f9e43324e99428ULL,
	0x2cffe7d5975e55e2ULL, 0x6e0f063e3eb46371ULL,
	0xa91e2402c48a38c4ULL, 0xebeec5e96d600e57ULL,
	0x65cc8190991cb93dULL, 0x273c607b30f68faeULL,
	0xe02d4247cac8d41bULL, 0xa2dda3ac6322e288ULL,
	0xbe992b5f8bdb8c5cULL, 0xfc69cab42231bacfULL,
	0x3b78e888d80fe17aULL, 0x7988096371e5d7e9ULL,
	0xf7aa4d1a85996083ULL, 0xb55aacf12c735610ULL,
	0x724b8ecdd64d0da5ULL, 0x30bb6f267fa73b36ULL,
	0x4ac29f2a07bfd00dULL, 0x08327ec1ae55e69eULL,
	0xcf235cfd546bbd2bULL, 0x8dd3bd16fd818bb8ULL,
	0x03f1f96f09fd3cd2ULL, 0x41011884a0170a41ULL,
	0x86103ab85a2951f4ULL, 0xc4e0db53f3c36767ULL,
	0xd8a453a01b3a09b3ULL, 0x9a54b24bb2d03f20ULL,
	0x5d45907748ee6495ULL, 0x1fb5719ce1045206ULL,
	0x919735e51578e56cULL, 0xd367d40ebc92d3ffULL,
	0x1476f63246ac884aULL, 0x568617d9ef46bed9ULL,
	0xe085162ab69d5e3cULL, 0xa275f7c11f7768afULL,
	0x6564d5fde549331aULL, 0x279434164ca30589ULL,
	0xa9b6706fb8dfb2e3ULL, 0xeb46918411358470ULL,
	0x2c57b3b8eb0bdfc5ULL, 0x6ea7525342e1e956ULL,
	0x72e3daa0aa188782ULL, 0x30133b4b03f2b111ULL,
	0xf7021977f9cceaa4ULL, 0xb5f2f89c5026dc37ULL,
	0x3bd0bce5a45a6b5dULL, 0x79205d0e0db05dceULL,
	0xbe317f32f78e067bULL, 0xfcc19ed95e6430e8ULL,
	0x86b86ed5267cdbd3ULL, 0xc4488f3e8f96ed40ULL,
	0x0359ad0275a8b6f5ULL, 0x41a94ce9dc428066ULL,
	0xcf8b0890283e370cULL, 0x8d7be97b81d4019fULL,
	0x4a6acb477bea5a2aULL, 0x089a2aacd2006cb9ULL,
	0x14dea25f3af9026dULL, 0x562e43b4931334feULL,
	0x913f6188692d6f4bULL, 0xd3cf8063c0c759d8ULL,
	0x5dedc41a34bbeeb2ULL, 0x1f1d25f19d51d821ULL,
	0xd80c07cd676f8394ULL, 0x9afce626ce85b507ULL
};

static const uint64_t crc64_iso_refl_table[256] = {
	0x0000000000000000ULL, 0x01b0000000000000ULL,
	0x0360000000000000ULL, 0x02d0000000000000ULL,
	0x06c0000000000000ULL, 0x0770000000000000ULL,
	0x05a0000000000000ULL, 0x0410000000000000ULL,
	0x0d80000000000000ULL, 0x0c30000000000000ULL,
	0x0ee0000000000000ULL, 0x0f50000000000000ULL,
	0x0b40000000000000ULL, 0x0af0000000000000ULL,
	0x0820000000000000ULL, 0x0990000000000000ULL,
	0x1b00000000000000ULL, 0x1ab0000000000000ULL,
	0x1860000000000000ULL, 0x19d0000000000000ULL,
	0x1dc0000000000000ULL, 0x1c70000000000000ULL,
	0x1ea0000000000000ULL, 0x1f10000000000000ULL,
	0x1680000000000000ULL, 0x1730000000000000ULL,
	0x15e0000000000000ULL, 0x1450000000000000ULL,
	0x1040000000000000ULL, 0x11f0000000000000ULL,
	0x1320000000000000ULL, 0x1290000000000000ULL,
	0x3600000000000000ULL, 0x37b0000000000000ULL,
	0x3560000000000000ULL, 0x34d0000000000000ULL,
	0x30c0000000000000ULL, 0x3170000000000000ULL,
	0x33a0000000000000ULL, 0x3210000000000000ULL,
	0x3b80000000000000ULL, 0x3a30000000000000ULL,
	0x38e0000000000000ULL, 0x3950000000000000ULL,
	0x3d40000000000000ULL, 0x3cf0000000000000ULL,
	0x3e20000000000000ULL, 0x3f90000000000000ULL,
	0x2d00000000000000ULL, 0x2cb0000000000000ULL,
	0x2e60000000000000ULL, 0x2fd0000000000000ULL,
	0x2bc0000000000000ULL, 0x2a70000000000000ULL,
	0x28a0000000000000ULL, 0x2910000000000000ULL,
	0x2080000000000000ULL, 0x2130000000000000ULL,
	0x23e0000000000000ULL, 0x2250000000000000ULL,
	0x2640000000000000ULL, 0x27f0000000000000ULL,
	0x2520000000000000ULL, 0x2490000000000000ULL,
	0x6c00000000000000ULL, 0x6db0000000000000ULL,
	0x6f60000000000000ULL, 0x6ed0000000000000ULL,
	0x6ac0000000000000ULL, 0x6b70000000000000ULL,
	0x69a0000000000000ULL, 0x6810000000000000ULL,
	0x6180000000000000ULL, 0x6030000000000000ULL,
	0x62e0000000000000ULL, 0x6350000000000000ULL,
	0x6740000000000000ULL, 0x66f0000000000000ULL,
	0x6420000000000000ULL, 0x6590000000000000ULL,
	0x7700000000000000ULL, 0x76b0000000000000ULL,
	0x7460000000000000ULL, 0x75d0000000000000ULL,
	0x71c0000000000000ULL, 0x7070000000000000ULL,
	0x72a0000000000000ULL, 0x7310000000000000ULL,
	0x7a80000000000000ULL, 0x7b30000000000000ULL,
	0x79e0000000000000ULL, 0x7850000000000000ULL,
	0x7c40000000000000ULL, 0x7df0000000000000ULL,
	0x7f20000000000000ULL, 0x7e90000000000000ULL,
	0x5a00000000000000ULL, 0x5bb0000000000000ULL,
	0x5960000000000000ULL, 0x58d0000000000000ULL,
	0x5cc0000000000000ULL, 0x5d70000000000000ULL,
	0x5fa0000000000000ULL, 0x5e10000000000000ULL,
	0x5780000000000000ULL, 0x5630000000000000ULL,
	0x54e0000000000000ULL, 0x5550000000000000ULL,
	0x5140000000000000ULL, 0x50f0000000000000ULL,
	0x5220000000000000ULL, 0x5390000000000000ULL,
	0x4100000000000000ULL, 0x40b0000000000000ULL,
	0x4260000000000000ULL, 0x43d0000000000000ULL,
	0x47c0000000000000ULL, 0x4670000000000000ULL,
	0x44a0000000000000ULL, 0x4510000000000000ULL,
	0x4c80000000000000ULL, 0x4d30000000000000ULL,
	0x4fe0000000000000ULL, 0x4e50000000000000ULL,
	0x4a40000000000000ULL, 0x4bf0000000000000ULL,
	0x4920000000000000ULL, 0x4890000000000000ULL,
	0xd800000000000000ULL, 0xd9b0000000000000ULL,
	0xdb60000000000000ULL, 0xdad0000000000000ULL,
	0xdec0000000000000ULL, 0xdf70000000000000ULL,
	0xdda0000000000000ULL, 0xdc10000000000000ULL,
	0xd580000000000000ULL, 0xd430000000000000ULL,
	0xd6e0000000000000ULL, 0xd750000000000000ULL,
	0xd340000000000000ULL, 0xd2f0000000000000ULL,
	0xd020000000000000ULL, 0xd190000000000000ULL,
	0xc300000000000000ULL, 0xc2b0000000000000ULL,
	0xc060000000000000ULL, 0xc1d0000000000000ULL,
	0xc5c0000000000000ULL, 0xc470000000000000ULL,
	0xc6a0000000000000ULL, 0xc710000000000000ULL,
	0xce80000000000000ULL, 0xcf30000000000000ULL,
	0xcde0000000000000ULL, 0xcc50000000000000ULL,
	0xc840000000000000ULL, 0xc9f0000000000000ULL,
	0xcb20000000000000ULL, 0xca90000000000000ULL,
	0xee00000000000000ULL, 0xefb0000000000000ULL,
	0xed60000000000000ULL, 0xecd0000000000000ULL,
	0xe8c0000000000000ULL, 0xe970000000000000ULL,
	0xeba0000000000000ULL, 0xea10000000000000ULL,
	0xe380000000000000ULL, 0xe230000000000000ULL,
	0xe0e0000000000000ULL, 0xe150000000000000ULL,
	0xe540000000000000ULL, 0xe4f0000000000000ULL,
	0xe620000000000000ULL, 0xe790000000000000ULL,
	0xf500000000000000ULL, 0xf4b0000000000000ULL,
	0xf660000000000000ULL, 0xf7d0000000000000ULL,
	0xf3c0000000000000ULL, 0xf270000000000000ULL,
	0xf0a0000000000000ULL, 0xf110000000000000ULL,
	0xf880000000000000ULL, 0xf930000000000000ULL,
	0xfbe0000000000000ULL, 0xfa50000000000000ULL,
	0xfe40000000000000ULL, 0xfff0000000000000ULL,
	0xfd20000000000000ULL, 0xfc90000000000000ULL,
	0xb400000000000000ULL, 0xb5b0000000000000ULL,
	0xb760000000000000ULL, 0xb6d0000000000000ULL,
	0xb2c0000000000000ULL, 0xb370000000000000ULL,
	0xb1a0000000000000ULL, 0xb010000000000000ULL,
	0xb980000000000000ULL, 0xb830000000000000ULL,
	0xbae0000000000000ULL, 0xbb50000000000000ULL,
	0xbf40000000000000ULL, 0xbef0000000000000ULL,
	0xbc20000000000000ULL, 0xbd90000000000000ULL,
	0xaf00000000000000ULL, 0xaeb0000000000000ULL,
	0xac60000000000000ULL, 0xadd0000000000000ULL,
	0xa9c0000000000000ULL, 0xa870000000000000ULL,
	0xaaa0000000000000ULL, 0xab10000000000000ULL,
	0xa280000000000000ULL, 0xa330000000000000ULL,
	0xa1e0000000000000ULL, 0xa050000000000000ULL,
	0xa440000000000000ULL, 0xa5f0000000000000ULL,
	0xa720000000000000ULL, 0xa690000000000000ULL,
	0x8200000000000000ULL, 0x83b0000000000000ULL,
	0x8160000000000000ULL, 0x80d0000000000000ULL,
	0x84c0000000000000ULL, 0x8570000000000000ULL,
	0x87a0000000000000ULL, 0x8610000000000000ULL,
	0x8f80000000000000ULL, 0x8e30000000000000ULL,
	0x8ce0000000000000ULL, 0x8d50000000000000ULL,
	0x8940000000000000ULL, 0x88f0000000000000ULL,
	0x8a20000000000000ULL, 0x8b90000000000000ULL,
	0x9900000000000000ULL, 0x98b0000000000000ULL,
	0x9a60000000000000ULL, 0x9bd0000000000000ULL,
	0x9fc0000000000000ULL, 0x9e70000000000000ULL,
	0x9ca0000000000000ULL, 0x9d10000000000000ULL,
	0x9480000000000000ULL, 0x9530000000000000ULL,
	0x97e0000000000000ULL, 0x9650000000000000ULL,
	0x9240000000000000ULL, 0x93f0000000000000ULL,
	0x9120000000000000ULL, 0x9090000000000000ULL
};

static const uint64_t crc64_iso_norm_table[256] = {
	0x0000000000000000ULL, 0x000000000000001bULL,
	0x0000000000000036ULL, 0x000000000000002dULL,
	0x000000000000006cULL, 0x0000000000000077ULL,
	0x000000000000005aULL, 0x0000000000000041ULL,
	0x00000000000000d8ULL, 0x00000000000000c3ULL,
	0x00000000000000eeULL, 0x00000000000000f5ULL,
	0x00000000000000b4ULL, 0x00000000000000afULL,
	0x0000000000000082ULL, 0x0000000000000099ULL,
	0x00000000000001b0ULL, 0x00000000000001abULL,
	0x0000000000000186ULL, 0x000000000000019dULL,
	0x00000000000001dcULL, 0x00000000000001c7ULL,
	0x00000000000001eaULL, 0x00000000000001f1ULL,
	0x0000000000000168ULL, 0x0000000000000173ULL,
	0x000000000000015eULL, 0x0000000000000145ULL,
	0x0000000000000104ULL, 0x000000000000011fULL,
	0x0000000000000132ULL, 0x0000000000000129ULL,
	0x0000000000000360ULL, 0x000000000000037bULL,
	0x0000000000000356ULL, 0x000000000000034dULL,
	0x000000000000030cULL, 0x0000000000000317ULL,
	0x000000000000033aULL, 0x0000000000000321ULL,
	0x00000000000003b8ULL, 0x00000000000003a3ULL,
	0x000000000000038eULL, 0x0000000000000395ULL,
	0x00000000000003d4ULL, 0x00000000000003cfULL,
	0x00000000000003e2ULL, 0x00000000000003f9ULL,
	0x00000000000002d0ULL, 0x00000000000002cbULL,
	0x00000000000002e6ULL, 0x00000000000002fdULL,
	0x00000000000002bcULL, 0x00000000000002a7ULL,
	0x000000000000028aULL, 0x0000000000000291ULL,
	0x0000000000000208ULL, 0x0000000000000213ULL,
	0x000000000000023eULL, 0x0000000000000225ULL,
	0x0000000000000264ULL, 0x000000000000027fULL,
	0x0000000000000252ULL, 0x0000000000000249ULL,
	0x00000000000006c0ULL, 0x00000000000006dbULL,
	0x00000000000006f6ULL, 0x00000000000006edULL,
	0x00000000000006acULL, 0x00000000000006b7ULL,
	0x000000000000069aULL, 0x0000000000000681ULL,
	0x0000000000000618ULL, 0x0000000000000603ULL,
	0x000000000000062eULL, 0x0000000000000635ULL,
	0x0000000000000674ULL, 0x000000000000066fULL,
	0x0000000000000642ULL, 0x0000000000000659ULL,
	0x0000000000000770ULL, 0x000000000000076bULL,
	0x0000000000000746ULL, 0x000000000000075dULL,
	0x000000000000071cULL, 0x0000000000000707ULL,
	0x000000000000072aULL, 0x0000000000000731ULL,
	0x00000000000007a8ULL, 0x00000000000007b3ULL,
	0x000000000000079eULL, 0x0000000000000785ULL,
	0x00000000000007c4ULL, 0x00000000000007dfULL,
	0x00000000000007f2ULL, 0x00000000000007e9ULL,
	0x00000000000005a0ULL, 0x00000000000005bbULL,
	0x0000000000000596ULL, 0x000000000000058dULL,
	0x00000000000005ccULL, 0x00000000000005d7ULL,
	0x00000000000005faULL, 0x00000000000005e1ULL,
	0x0000000000000578ULL, 0x0000000000000563ULL,
	0x000000000000054eULL, 0x0000000000000555ULL,
	0x0000000000000514ULL, 0x000000000000050fULL,
	0x0000000000000522ULL, 0x0000000000000539ULL,
	0x0000000000000410ULL, 0x000000000000040bULL,
	0x0000000000000426ULL, 0x000000000000043dULL,
	0x000000000000047cULL, 0x0000000000000467ULL,
	0x000000000000044aULL, 0x0000000000000451ULL,
	0x00000000000004c8ULL, 0x00000000000004d3ULL,
	0x00000000000004feULL, 0x00000000000004e5ULL,
	0x00000000000004a4ULL, 0x00000000000004bfULL,
	0x0000000000000492ULL, 0x0000000000000489ULL,
	0x0000000000000d80ULL, 0x0000000000000d9bULL,
	0x0000000000000db6ULL, 0x0000000000000dadULL,
	0x0000000000000decULL, 0x0000000000000df7ULL,
	0x0000000000000ddaULL, 0x0000000000000dc1ULL,
	0x0000000000000d58ULL, 0x0000000000000d43ULL,
	0x0000000000000d6eULL, 0x0000000000000d75ULL,
	0x0000000000000d34ULL, 0x0000000000000d2fULL,
	0x0000000000000d02ULL, 0x0000000000000d19ULL,
	0x0000000000000c30ULL, 0x0000000000000c2bULL,
	0x0000000000000c06ULL, 0x0000000000000c1dULL,
	0x0000000000000c5cULL, 0x0000000000000c47ULL,
	0x0000000000000c6aULL, 0x0000000000000c71ULL,
	0x0000000000000ce8ULL, 0x0000000000000cf3ULL,
	0x0000000000000cdeULL, 0x0000000000000cc5ULL,
	0x0000000000000c84ULL, 0x0000000000000c9fULL,
	0x0000000000000cb2ULL, 0x0000000000000ca9ULL,
	0x0000000000000ee0ULL, 0x0000000000000efbULL,
	0x0000000000000ed6ULL, 0x0000000000000ecdULL,
	0x0000000000000e8cULL, 0x0000000000000e97ULL,
	0x0000000000000ebaULL, 0x0000000000000ea1ULL,
	0x0000000000000e38ULL, 0x0000000000000e23ULL,
	0x0000000000000e0eULL, 0x0000000000000e15ULL,
	0x0000000000000e54ULL, 0x0000000000000e4fULL,
	0x0000000000000e62ULL, 0x0000000000000e79ULL,
	0x0000000000000f50ULL, 0x0000000000000f4bULL,
	0x0000000000000f66ULL, 0x0000000000000f7dULL,
	0x0000000000000f3cULL, 0x0000000000000f27ULL,
	0x0000000000000f0aULL, 0x0000000000000f11ULL,
	0x0000000000000f88ULL, 0x0000000000000f93ULL,
	0x0000000000000fbeULL, 0x0000000000000fa5ULL,
	0x0000000000000fe4ULL, 0x0000000000000fffULL,
	0x0000000000000fd2ULL, 0x0000000000000fc9ULL,
	0x0000000000000b40ULL, 0x0000000000000b5bULL,
	0x0000000000000b76ULL, 0x0000000000000b6dULL,
	0x0000000000000b2cULL, 0x0000000000000b37ULL,
	0x0000000000000b1aULL, 0x0000000000000b01ULL,
	0x0000000000000b98ULL, 0x0000000000000b83ULL,
	0x0000000000000baeULL, 0x0000000000000bb5ULL,
	0x0000000000000bf4ULL, 0x0000000000000befULL,
	0x0000000000000bc2ULL, 0x0000000000000bd9ULL,
	0x0000000000000af0ULL, 0x0000000000000aebULL,
	0x0000000000000ac6ULL, 0x0000000000000addULL,
	0x0000000000000a9cULL, 0x0000000000000a87ULL,
	0x0000000000000aaaULL, 0x0000000000000ab1ULL,
	0x0000000000000a28ULL, 0x0000000000000a33ULL,
	0x0000000000000a1eULL, 0x0000000000000a05ULL,
	0x0000000000000a44ULL, 0x0000000000000a5fULL,
	0x0000000000000a72ULL, 0x0000000000000a69ULL,
	0x0000000000000820ULL, 0x000000000000083bULL,
	0x0000000000000816ULL, 0x000000000000080dULL,
	0x000000000000084cULL, 0x0000000000000857ULL,
	0x000000000000087aULL, 0x0000000000000861ULL,
	0x00000000000008f8ULL, 0x00000000000008e3ULL,
	0x00000000000008ceULL, 0x00000000000008d5ULL,
	0x0000000000000894ULL, 0x000000000000088fULL,
	0x00000000000008a2ULL, 0x00000000000008b9ULL,
	0x0000000000000990ULL, 0x000000000000098bULL,
	0x00000000000009a6ULL, 0x00000000000009bdULL,
	0x00000000000009fcULL, 0x00000000000009e7ULL,
	0x00000000000009caULL, 0x00000000000009d1ULL,
	0x0000000000000948ULL, 0x0000000000000953ULL,
	0x000000000000097eULL, 0x0000000000000965ULL,
	0x0000000000000924ULL, 0x000000000000093fULL,
	0x0000000000000912ULL, 0x0000000000000909ULL
};

static const uint64_t crc64_jones_refl_table[256] = {
	0x0000000000000000ULL, 0x7ad870c830358979ULL,
	0xf5b0e190606b12f2ULL, 0x8f689158505e9b8bULL,
	0xc038e5739841b68fULL, 0xbae095bba8743ff6ULL,
	0x358804e3f82aa47dULL, 0x4f50742bc81f2d04ULL,
	0xab28ecb46814fe75ULL, 0xd1f09c7c5821770cULL,
	0x5e980d24087fec87ULL, 0x24407dec384a65feULL,
	0x6b1009c7f05548faULL, 0x11c8790fc060c183ULL,
	0x9ea0e857903e5a08ULL, 0xe478989fa00bd371ULL,
	0x7d08ff3b88be6f81ULL, 0x07d08ff3b88be6f8ULL,
	0x88b81eabe8d57d73ULL, 0xf2606e63d8e0f40aULL,
	0xbd301a4810ffd90eULL, 0xc7e86a8020ca5077ULL,
	0x4880fbd87094cbfcULL, 0x32588b1040a14285ULL,
	0xd620138fe0aa91f4ULL, 0xacf86347d09f188dULL,
	0x2390f21f80c18306ULL, 0x594882d7b0f40a7fULL,
	0x1618f6fc78eb277bULL, 0x6cc0863448deae02ULL,
	0xe3a8176c18803589ULL, 0x997067a428b5bcf0ULL,
	0xfa11fe77117cdf02ULL, 0x80c98ebf2149567bULL,
	0x0fa11fe77117cdf0ULL, 0x75796f2f41224489ULL,
	0x3a291b04893d698dULL, 0x40f16bccb908e0f4ULL,
	0xcf99fa94e9567b7fULL, 0xb5418a5cd963f206ULL,
	0x513912c379682177ULL, 0x2be1620b495da80eULL,
	0xa489f35319033385ULL, 0xde51839b2936bafcULL,
	0x9101f7b0e12997f8ULL, 0xebd98778d11c1e81ULL,
	0x64b116208142850aULL, 0x1e6966e8b1770c73ULL,
	0x8719014c99c2b083ULL, 0xfdc17184a9f739faULL,
	0x72a9e0dcf9a9a271ULL, 0x08719014c99c2b08ULL,
	0x4721e43f0183060cULL, 0x3df994f731b68f75ULL,
	0xb29105af61e814feULL, 0xc849756751dd9d87ULL,
	0x2c31edf8f1d64ef6ULL, 0x56e99d30c1e3c78fULL,
	0xd9810c6891bd5c04ULL, 0xa3597ca0a188d57dULL,
	0xec09088b6997f879ULL, 0x96d1784359a27100ULL,
	0x19b9e91b09fcea8bULL, 0x636199d339c963f2ULL,
	0xdf7adabd7a6e2d6fULL, 0xa5a2aa754a5ba416ULL,
	0x2aca3b2d1a053f9dULL, 0x50124be52a30b6e4ULL,
	0x1f423fcee22f9be0ULL, 0x659a4f06d21a1299ULL,
	0xeaf2de5e82448912ULL, 0x902aae96b271006bULL,
	0x74523609127ad31aULL, 0x0e8a46c1224f5a63ULL,
	0x81e2d7997211c1e8ULL, 0xfb3aa75142244891ULL,
	0xb46ad37a8a3b6595ULL, 0xceb2a3b2ba0eececULL,
	0x41da32eaea507767ULL, 0x3b024222da65fe1eULL,
	0xa2722586f2d042eeULL, 0xd8aa554ec2e5cb97ULL,
	0x57c2c41692bb501cULL, 0x2d1ab4dea28ed965ULL,
	0x624ac0f56a91f461ULL, 0x1892b03d5aa47d18ULL,
	0x97fa21650afae693ULL, 0xed2251ad3acf6feaULL,
	0x095ac9329ac4bc9bULL, 0x7382b9faaaf135e2ULL,
	0xfcea28a2faafae69ULL, 0x8632586aca9a2710ULL,
	0xc9622c4102850a14ULL, 0xb3ba5c8932b0836dULL,
	0x3cd2cdd162ee18e6ULL, 0x460abd1952db919fULL,
	0x256b24ca6b12f26dULL, 0x5fb354025b277b14ULL,
	0xd0dbc55a0b79e09fULL, 0xaa03b5923b4c69e6ULL,
	0xe553c1b9f35344e2ULL, 0x9f8bb171c366cd9bULL,
	0x10e3202993385610ULL, 0x6a3b50e1a30ddf69ULL,
	0x8e43c87e03060c18ULL, 0xf49bb8b633338561ULL,
	0x7bf329ee636d1eeaULL, 0x012b592653589793ULL,
	0x4e7b2d0d9b47ba97ULL, 0x34a35dc5ab7233eeULL,
	0xbbcbcc9dfb2ca865ULL, 0xc113bc55cb19211cULL,
	0x5863dbf1e3ac9decULL, 0x22bbab39d3991495ULL,
	0xadd33a6183c78f1eULL, 0xd70b4aa9b3f20667ULL,
	0x985b3e827bed2b63ULL, 0xe2834e4a4bd8a21aULL,
	0x6debdf121b863991ULL, 0x1733afda2bb3b0e8ULL,
	0xf34b37458bb86399ULL, 0x8993478dbb8deae0ULL,
	0x06fbd6d5ebd3716bULL, 0x7c23a61ddbe6f812ULL,
	0x3373d23613f9d516ULL, 0x49aba2fe23cc5c6fULL,
	0xc6c333a67392c7e4ULL, 0xbc1b436e43a74e9dULL,
	0x95ac9329ac4bc9b5ULL, 0xef74e3e19c7e40ccULL,
	0x601c72b9cc20db47ULL, 0x1ac40271fc15523eULL,
	0x5594765a340a7f3aULL, 0x2f4c0692043ff643ULL,
	0xa02497ca54616dc8ULL, 0xdafce7026454e4b1ULL,
	0x3e847f9dc45f37c0ULL, 0x445c0f55f46abeb9ULL,
	0xcb349e0da4342532ULL, 0xb1eceec59401ac4bULL,
	0xfebc9aee5c1e814fULL, 0x8464ea266c2b0836ULL,
	0x0b0c7b7e3c7593bdULL, 0x71d40bb60c401ac4ULL,
	0xe8a46c1224f5a634ULL, 0x927c1cda14c02f4dULL,
	0x1d148d82449eb4c6ULL, 0x67ccfd4a74ab3dbfULL,
	0x289c8961bcb410bbULL, 0x5244f9a98c8199c2ULL,
	0xdd2c68f1dcdf0249ULL, 0xa7f41839ecea8b30ULL,
	0x438c80a64ce15841ULL, 0x3954f06e7cd4d138ULL,
	0xb63c61362c8a4ab3ULL, 0xcce411fe1cbfc3caULL,
	0x83b465d5d4a0eeceULL, 0xf96c151de49567b7ULL,
	0x76048445b4cbfc3cULL, 0x0cdcf48d84fe7545ULL,
	0x6fbd6d5ebd3716b7ULL, 0x15651d968d029fceULL,
	0x9a0d8ccedd5c0445ULL, 0xe0d5fc06ed698d3cULL,
	0xaf85882d2576a038ULL, 0xd55df8e515432941ULL,
	0x5a3569bd451db2caULL, 0x20ed197575283bb3ULL,
	0xc49581ead523e8c2ULL, 0xbe4df122e51661bbULL,
	0x3125607ab548fa30ULL, 0x4bfd10b2857d7349ULL,
	0x04ad64994d625e4dULL, 0x7e7514517d57d734ULL,
	0xf11d85092d094cbfULL, 0x8bc5f5c11d3cc5c6ULL,
	0x12b5926535897936ULL, 0x686de2ad05bcf04fULL,
	0xe70573f555e26bc4ULL, 0x9ddd033d65d7e2bdULL,
	0xd28d7716adc8cfb9ULL, 0xa85507de9dfd46c0ULL,
	0x273d9686cda3dd4bULL, 0x5de5e64efd965432ULL,
	0xb99d7ed15d9d8743ULL, 0xc3450e196da80e3aULL,
	0x4c2d9f413df695b1ULL, 0x36f5ef890dc31cc8ULL,
	0x79a59ba2c5dc31ccULL, 0x037deb6af5e9b8b5ULL,
	0x8c157a32a5b7233eULL, 0xf6cd0afa9582aa47ULL,
	0x4ad64994d625e4daULL, 0x300e395ce6106da3ULL,
	0xbf66a804b64ef628ULL, 0xc5bed8cc867b7f51ULL,
	0x8aeeace74e645255ULL, 0xf036dc2f7e51db2cULL,
	0x7f5e4d772e0f40a7ULL, 0x05863dbf1e3ac9deULL,
	0xe1fea520be311aafULL, 0x9b26d5e88e0493d6ULL,
	0x144e44b0de5a085dULL, 0x6e963478ee6f8124ULL,
	0x21c640532670ac20ULL, 0x5b1e309b16452559ULL,
	0xd476a1c3461bbed2ULL, 0xaeaed10b762e37abULL,
	0x37deb6af5e9b8b5bULL, 0x4d06c6676eae0222ULL,
	0xc26e573f3ef099a9ULL, 0xb8b627f70ec510d0ULL,
	0xf7e653dcc6da3dd4ULL, 0x8d3e2314f6efb4adULL,
	0x0256b24ca6b12f26ULL, 0x788ec2849684a65fULL,
	0x9cf65a1b368f752eULL, 0xe62e2ad306bafc57ULL,
	0x6946bb8b56e467dcULL, 0x139ecb4366d1eea5ULL,
	0x5ccebf68aecec3a1ULL, 0x2616cfa09efb4ad8ULL,
	0xa97e5ef8cea5d153ULL, 0xd3a62e30fe90582aULL,
	0xb0c7b7e3c7593bd8ULL, 0xca1fc72bf76cb2a1ULL,
	0x45775673a732292aULL, 0x3faf26bb9707a053ULL,
	0x70ff52905f188d57ULL, 0x0a2722586f2d042eULL,
	0x854fb3003f739fa5ULL, 0xff97c3c80f4616dcULL,
	0x1bef5b57af4dc5adULL, 0x61372b9f9f784cd4ULL,
	0xee5fbac7cf26d75fULL, 0x9487ca0fff135e26ULL,
	0xdbd7be24370c7322ULL, 0xa10fceec0739fa5bULL,
	0x2e675fb4576761d0ULL, 0x54bf2f7c6752e8a9ULL,
	0xcdcf48d84fe75459ULL, 0xb71738107fd2dd20ULL,
	0x387fa9482f8c46abULL, 0x42a7d9801fb9cfd2ULL,
	0x0df7adabd7a6e2d6ULL, 0x772fdd63e7936bafULL,
	0xf8474c3bb7cdf024ULL, 0x829f3cf387f8795dULL,
	0x66e7a46c27f3aa2cULL, 0x1c3fd4a417c62355ULL,
	0x935745fc4798b8deULL, 0xe98f353477ad31a7ULL,
	0xa6df411fbfb21ca3ULL, 0xdc0731d78f8795daULL,
	0x536fa08fdfd90e51ULL, 0x29b7d047efec8728ULL
};

static const uint64_t crc64_jones_norm_table[256] = {
	0x0000000000000000ULL, 0xad93d23594c935a9ULL,
	0xf6b4765ebd5b5efbULL, 0x5b27a46b29926b52ULL,
	0x40fb3e88ee7f885fULL, 0xed68ecbd7ab6bdf6ULL,
	0xb64f48d65324d6a4ULL, 0x1bdc9ae3c7ede30dULL,
	0x81f67d11dcff10beULL, 0x2c65af2448362517ULL,
	0x77420b4f61a44e45ULL, 0xdad1d97af56d7becULL,
	0xc10d4399328098e1ULL, 0x6c9e91aca649ad48ULL,
	0x37b935c78fdbc61aULL, 0x9a2ae7f21b12f3b3ULL,
	0xae7f28162d3714d5ULL, 0x03ecfa23b9fe217cULL,
	0x58cb5e48906c4a2eULL, 0xf5588c7d04a57f87ULL,
	0xee84169ec3489c8aULL, 0x4317c4ab5781a923ULL,
	0x183060c07e13c271ULL, 0xb5a3b2f5eadaf7d8ULL,
	0x2f895507f1c8046bULL, 0x821a8732650131c2ULL,
	0xd93d23594c935a90ULL, 0x74aef16cd85a6f39ULL,
	0x6f726b8f1fb78c34ULL, 0xc2e1b9ba8b7eb99dULL,
	0x99c61dd1a2ecd2cfULL, 0x3455cfe43625e766ULL,
	0xf16d8219cea71c03ULL, 0x5cfe502c5a6e29aaULL,
	0x07d9f44773fc42f8ULL, 0xaa4a2672e7357751ULL,
	0xb196bc9120d8945cULL, 0x1c056ea4b411a1f5ULL,
	0x4722cacf9d83caa7ULL, 0xeab118fa094aff0eULL,
	0x709bff0812580cbdULL, 0xdd082d3d86913914ULL,
	0x862f8956af035246ULL, 0x2bbc5b633bca67efULL,
	0x3060c180fc2784e2ULL, 0x9df313b568eeb14bULL,
	0xc6d4b7de417cda19ULL, 0x6b4765ebd5b5efb0ULL,
	0x5f12aa0fe39008d6ULL, 0xf281783a77593d7fULL,
	0xa9a6dc515ecb562dULL, 0x04350e64ca026384ULL,
	0x1fe994870def8089ULL, 0xb27a46b29926b520ULL,
	0xe95de2d9b0b4de72ULL, 0x44ce30ec247debdbULL,
	0xdee4d71e3f6f1868ULL, 0x7377052baba62dc1ULL,
	0x2850a14082344693ULL, 0x85c3737516fd733aULL,
	0x9e1fe996d1109037ULL, 0x338c3ba345d9a59eULL,
	0x68ab9fc86c4bceccULL, 0xc5384dfdf882fb65ULL,
	0x4f48d60609870dafULL, 0xe2db04339d4e3806ULL,
	0xb9fca058b4dc5354ULL, 0x146f726d201566fdULL,
	0x0fb3e88ee7f885f0ULL, 0xa2203abb7331b059ULL,
	0xf9079ed05aa3db0bULL, 0x54944ce5ce6aeea2ULL,
	0xcebeab17d5781d11ULL, 0x632d792241b128b8ULL,
	0x380add49682343eaULL, 0x95990f7cfcea7643ULL,
	0x8e45959f3b07954eULL, 0x23d647aaafcea0e7ULL,
	0x78f1e3c1865ccbb5ULL, 0xd56231f41295fe1cULL,
	0xe137fe1024b0197aULL, 0x4ca42c25b0792cd3ULL,
	0x1783884e99eb4781ULL, 0xba105a7b0d227228ULL,
	0xa1ccc098cacf9125ULL, 0x0c5f12ad5e06a48cULL,
	0x5778b6c67794cfdeULL, 0xfaeb64f3e35dfa77ULL,
	0x60c18301f84f09c4ULL, 0xcd5251346c863c6dULL,
	0x9675f55f4514573fULL, 0x3be6276ad1dd6296ULL,
	0x203abd891630819bULL, 0x8da96fbc82f9b432ULL,
	0xd68ecbd7ab6bdf60ULL, 0x7b1d19e23fa2eac9ULL,
	0xbe25541fc72011acULL, 0x13b6862a53e92405ULL,
	0x489122417a7b4f57ULL, 0xe502f074eeb27afeULL,
	0xfede6a97295f99f3ULL, 0x534db8a2bd96ac5aULL,
	0x086a1cc99404c708ULL, 0xa5f9cefc00cdf2a1ULL,
	0x3fd3290e1bdf0112ULL, 0x9240fb3b8f1634bbULL,
	0xc9675f50a6845fe9ULL, 0x64f48d65324d6a40ULL,
	0x7f281786f5a0894dULL, 0xd2bbc5b36169bce4ULL,
	0x899c61d848fbd7b6ULL, 0x240fb3eddc32e21fULL,
	0x105a7c09ea170579ULL, 0xbdc9ae3c7ede30d0ULL,
	0xe6ee0a57574c5b82ULL, 0x4b7dd862c3856e2bULL,
	0x50a1428104688d26ULL, 0xfd3290b490a1b88fULL,
	0xa61534dfb933d3ddULL, 0x0b86e6ea2dfae674ULL,
	0x91ac011836e815c7ULL, 0x3c3fd32da221206eULL,
	0x671877468bb34b3cULL, 0xca8ba5731f7a7e95ULL,
	0xd1573f90d8979d98ULL, 0x7cc4eda54c5ea831ULL,
	0x27e349ce65ccc363ULL, 0x8a709bfbf105f6caULL,
	0x9e91ac0c130e1b5eULL, 0x33027e3987c72ef7ULL,
	0x6825da52ae5545a5ULL, 0xc5b608673a9c700cULL,
	0xde6a9284fd719301ULL, 0x73f940b169b8a6a8ULL,
	0x28dee4da402acdfaULL, 0x854d36efd4e3f853ULL,
	0x1f67d11dcff10be0ULL, 0xb2f403285b383e49ULL,
	0xe9d3a74372aa551bULL, 0x44407576e66360b2ULL,
	0x5f9cef95218e83bfULL, 0xf20f3da0b547b616ULL,
	0xa92899cb9cd5dd44ULL, 0x04bb4bfe081ce8edULL,
	0x30ee841a3e390f8bULL, 0x9d7d562faaf03a22ULL,
	0xc65af24483625170ULL, 0x6bc9207117ab64d9ULL,
	0x7015ba92d04687d4ULL, 0xdd8668a7448fb27dULL,
	0x86a1cccc6d1dd92fULL, 0x2b321ef9f9d4ec86ULL,
	0xb118f90be2c61f35ULL, 0x1c8b2b3e760f2a9cULL,
	0x47ac8f555f9d41ceULL, 0xea3f5d60cb547467ULL,
	0xf1e3c7830cb9976aULL, 0x5c7015b69870a2c3ULL,
	0x0757b1ddb1e2c991ULL, 0xaac463e8252bfc38ULL,
	0x6ffc2e15dda9075dULL, 0xc26ffc20496032f4ULL,
	0x9948584b60f259a6ULL, 0x34db8a7ef43b6c0fULL,
	0x2f07109d33d68f02ULL, 0x8294c2a8a71fbaabULL,
	0xd9b366c38e8dd1f9ULL, 0x7420b4f61a44e450ULL,
	0xee0a5304015617e3ULL, 0x43998131959f224aULL,
	0x18be255abc0d4918ULL, 0xb52df76f28c47cb1ULL,
	0xaef16d8cef299fbcULL, 0x0362bfb97be0aa15ULL,
	0x58451bd25272c147ULL, 0xf5d6c9e7c6bbf4eeULL,
	0xc1830603f09e1388ULL, 0x6c10d43664572621ULL,
	0x3737705d4dc54d73ULL, 0x9aa4a268d90c78daULL,
	0x8178388b1ee19bd7ULL, 0x2cebeabe8a28ae7eULL,
	0x77cc4ed5a3bac52cULL, 0xda5f9ce03773f085ULL,
	0x40757b122c610336ULL, 0xede6a927b8a8369fULL,
	0xb6c10d4c913a5dcdULL, 0x1b52df7905f36864ULL,
	0x008e459ac21e8b69ULL, 0xad1d97af56d7bec0ULL,
	0xf63a33c47f45d592ULL, 0x5ba9e1f1eb8ce03bULL,
	0xd1d97a0a1a8916f1ULL, 0x7c4aa83f8e402358ULL,
	0x276d0c54a7d2480aULL, 0x8afede61331b7da3ULL,
	0x91224482f4f69eaeULL, 0x3cb196b7603fab07ULL,
	0x679632dc49adc055ULL, 0xca05e0e9dd64f5fcULL,
	0x502f071bc676064fULL, 0xfdbcd52e52bf33e6ULL,
	0xa69b71457b2d58b4ULL, 0x0b08a370efe46d1dULL,
	0x10d4399328098e10ULL, 0xbd47eba6bcc0bbb9ULL,
	0xe6604fcd9552d0ebULL, 0x4bf39df8019be542ULL,
	0x7fa6521c37be0224ULL, 0xd2358029a377378dULL,
	0x891224428ae55cdfULL, 0x2481f6771e2c6976ULL,
	0x3f5d6c94d9c18a7bULL, 0x92cebea14d08bfd2ULL,
	0xc9e91aca649ad480ULL, 0x647ac8fff053e129ULL,
	0xfe502f0deb41129aULL, 0x53c3fd387f882733ULL,
	0x08e45953561a4c61ULL, 0xa5778b66c2d379c8ULL,
	0xbeab1185053e9ac5ULL, 0x1338c3b091f7af6cULL,
	0x481f67dbb865c43eULL, 0xe58cb5ee2cacf197ULL,
	0x20b4f813d42e0af2ULL, 0x8d272a2640e73f5bULL,
	0xd6008e4d69755409ULL, 0x7b935c78fdbc61a0ULL,
	0x604fc69b3a5182adULL, 0xcddc14aeae98b704ULL,
	0x96fbb0c5870adc56ULL, 0x3b6862f013c3e9ffULL,
	0xa142850208d11a4cULL, 0x0cd157379c182fe5ULL,
	0x57f6f35cb58a44b7ULL, 0xfa6521692143711eULL,
	0xe1b9bb8ae6ae9213ULL, 0x4c2a69bf7267a7baULL,
	0x170dcdd45bf5cce8ULL, 0xba9e1fe1cf3cf941ULL,
	0x8ecbd005f9191e27ULL, 0x235802306dd02b8eULL,
	0x787fa65b444240dcULL, 0xd5ec746ed08b7575ULL,
	0xce30ee8d17669678ULL, 0x63a33cb883afa3d1ULL,
	0x388498d3aa3dc883ULL, 0x95174ae63ef4fd2aULL,
	0x0f3dad1425e60e99ULL, 0xa2ae7f21b12f3b30ULL,
	0xf989db4a98bd5062ULL, 0x541a097f0c7465cbULL,
	0x4fc6939ccb9986c6ULL, 0xe25541a95f50b36fULL,
	0xb972e5c276c2d83dULL, 0x14e137f7e20bed94ULL
};

uint64_t crc64_ecma_refl_base(uint64_t seed, const uint8_t * buf, uint64_t len)
{
	uint64_t i, crc = ~seed;

	for (i = 0; i < len; i++) {
		uint8_t byte = buf[i];
		crc = crc64_ecma_refl_table[(uint8_t) crc ^ byte] ^ (crc >> 8);
	}

	return ~crc;
}

uint64_t crc64_ecma_norm_base(uint64_t seed, const uint8_t * buf, uint64_t len)
{
	uint64_t i, crc = ~seed;

	for (i = 0; i < len; i++) {
		uint8_t byte = buf[i];
		crc = crc64_ecma_norm_table[((crc >> 56) ^ byte) & 0xff] ^ (crc << 8);
	}

	return ~crc;
}

uint64_t crc64_iso_refl_base(uint64_t seed, const uint8_t * buf, uint64_t len)
{
	uint64_t i, crc = ~seed;

	for (i = 0; i < len; i++) {
		uint8_t byte = buf[i];
		crc = crc64_iso_refl_table[(uint8_t) crc ^ byte] ^ (crc >> 8);
	}

	return ~crc;
}

uint64_t crc64_iso_norm_base(uint64_t seed, const uint8_t * buf, uint64_t len)
{
	uint64_t i, crc = ~seed;

	for (i = 0; i < len; i++) {
		uint8_t byte = buf[i];
		crc = crc64_iso_norm_table[((crc >> 56) ^ byte) & 0xff] ^ (crc << 8);
	}

	return ~crc;
}

uint64_t crc64_jones_refl_base(uint64_t seed, const uint8_t * buf, uint64_t len)
{
	uint64_t i, crc = ~seed;

	for (i = 0; i < len; i++) {
		uint8_t byte = buf[i];
		crc = crc64_jones_refl_table[(uint8_t) crc ^ byte] ^ (crc >> 8);
	}

	return ~crc;
}

uint64_t crc64_jones_norm_base(uint64_t seed, const uint8_t * buf, uint64_t len)
{
	uint64_t i, crc = ~seed;

	for (i = 0; i < len; i++) {
		uint8_t byte = buf[i];
		crc = crc64_jones_norm_table[((crc >> 56) ^ byte) & 0xff] ^ (crc << 8);
	}

	return ~crc;
}

struct slver {
	unsigned short snum;
	unsigned char ver;
	unsigned char core;
};

struct slver crc64_ecma_refl_base_slver_0000001c;
struct slver crc64_ecma_refl_base_slver = { 0x001c, 0x00, 0x00 };

struct slver crc64_ecma_norm_base_slver_00000019;
struct slver crc64_ecma_norm_base_slver = { 0x0019, 0x00, 0x00 };

struct slver crc64_iso_refl_base_slver_00000022;
struct slver crc64_iso_refl_base_slver = { 0x0022, 0x00, 0x00 };

struct slver crc64_iso_norm_base_slver_0000001f;
struct slver crc64_iso_norm_base_slver = { 0x001f, 0x00, 0x00 };

struct slver crc64_jones_refl_base_slver_00000028;
struct slver crc64_jones_refl_base_slver = { 0x0028, 0x00, 0x00 };

struct slver crc64_jones_norm_base_slver_00000025;
struct slver crc64_jones_norm_base_slver = { 0x0025, 0x00, 0x00 };
