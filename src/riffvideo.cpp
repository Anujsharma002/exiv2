// ***************************************************************** -*- C++ -*-
/*
 * Copyright (C) 2004-2021 Exiv2 authors
 * This program is part of the Exiv2 distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * asize_t with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301 USA.
 */
// *****************************************************************************
// included header files
#include "config.h"

#include "basicio.hpp"
#include "error.hpp"
#include "futils.hpp"
#include "image_int.hpp"
#include "riffvideo.hpp"
#include "tags.hpp"
#include "tags_int.hpp"
#include "tiffimage_int.hpp"
#include "types.hpp"
// + standard includes
#include <cmath>

// *****************************************************************************
// class member definitions
namespace Exiv2::Internal {

/*!
  @brief Dummy TIFF header structure.
 */
class DummyTiffHeader : public TiffHeaderBase {
 public:
  //! @name Creators
  //@{
  //! Default constructor
  DummyTiffHeader(ByteOrder byteOrder);
  //! Destructor
  ~DummyTiffHeader() override = default;
  //@}

  //! @name Manipulators
  //@{
  //! Dummy read function. Does nothing and returns true.
  bool read(const byte* pData, size_t size) override;
  //@}

};  // class TiffHeader

DummyTiffHeader::DummyTiffHeader(ByteOrder byteOrder) : TiffHeaderBase(42, 0, byteOrder, 0) {
}

bool DummyTiffHeader::read(const byte* /*pData*/, size_t /*size*/) {
  return true;
}

constexpr TagVocabulary infoTags[] = {{"AGES", "Xmp.video.Rated"},
                                      {"CMNT", "Xmp.video.Comment"},
                                      {"CODE", "Xmp.video.EncodedBy"},
                                      {"COMM", "Xmp.video.Comment"},
                                      {"DIRC", "Xmp.video.Director"},
                                      {"DISP", "Xmp.audio.SchemeTitle"},
                                      {"DTIM", "Xmp.video.DateTimeOriginal"},
                                      {"GENR", "Xmp.video.Genre"},
                                      {"IARL", "Xmp.video.ArchivalLocation"},
                                      {"IART", "Xmp.video.Artist"},
                                      {"IAS1", "Xmp.video.Edit1"},
                                      {"IAS2", "Xmp.video.Edit2"},
                                      {"IAS3", "Xmp.video.Edit3"},
                                      {"IAS4", "Xmp.video.Edit4"},
                                      {"IAS5", "Xmp.video.Edit5"},
                                      {"IAS6", "Xmp.video.Edit6"},
                                      {"IAS7", "Xmp.video.Edit7"},
                                      {"IAS8", "Xmp.video.Edit8"},
                                      {"IAS9", "Xmp.video.Edit9"},
                                      {"IBSU", "Xmp.video.BaseURL"},
                                      {"ICAS", "Xmp.audio.DefaultStream"},
                                      {"ICDS", "Xmp.video.CostumeDesigner"},
                                      {"ICMS", "Xmp.video.Commissioned"},
                                      {"ICMT", "Xmp.video.Comment"},
                                      {"ICNM", "Xmp.video.Cinematographer"},
                                      {"ICNT", "Xmp.video.Country"},
                                      {"ICOP", "Xmp.video.Copyright"},
                                      {"ICRD", "Xmp.video.DateTimeDigitized"},
                                      {"ICRP", "Xmp.video.Cropped"},
                                      {"IDIM", "Xmp.video.Dimensions"},
                                      {"IDPI", "Xmp.video.DotsPerInch"},
                                      {"IDST", "Xmp.video.DistributedBy"},
                                      {"IEDT", "Xmp.video.EditedBy"},
                                      {"IENC", "Xmp.video.EncodedBy"},
                                      {"IENG", "Xmp.video.Engineer"},
                                      {"IGNR", "Xmp.video.Genre"},
                                      {"IKEY", "Xmp.video.PerformerKeywords"},
                                      {"ILGT", "Xmp.video.Lightness"},
                                      {"ILGU", "Xmp.video.LogoURL"},
                                      {"ILIU", "Xmp.video.LogoIconURL"},
                                      {"ILNG", "Xmp.video.Language"},
                                      {"IMBI", "Xmp.video.InfoBannerImage"},
                                      {"IMBU", "Xmp.video.InfoBannerURL"},
                                      {"IMED", "Xmp.video.Medium"},
                                      {"IMIT", "Xmp.video.InfoText"},
                                      {"IMIU", "Xmp.video.InfoURL"},
                                      {"IMUS", "Xmp.video.MusicBy"},
                                      {"INAM", "Xmp.video.Title"},
                                      {"IPDS", "Xmp.video.ProductionDesigner"},
                                      {"IPLT", "Xmp.video.NumOfColors"},
                                      {"IPRD", "Xmp.video.Product"},
                                      {"IPRO", "Xmp.video.ProducedBy"},
                                      {"IRIP", "Xmp.video.RippedBy"},
                                      {"IRTD", "Xmp.video.Rating"},
                                      {"ISBJ", "Xmp.video.Subject"},
                                      {"ISFT", "Xmp.video.Software"},
                                      {"ISGN", "Xmp.video.SecondaryGenre"},
                                      {"ISHP", "Xmp.video.Sharpness"},
                                      {"ISRC", "Xmp.video.Source"},
                                      {"ISRF", "Xmp.video.SourceForm"},
                                      {"ISTD", "Xmp.video.ProductionStudio"},
                                      {"ISTR", "Xmp.video.Starring"},
                                      {"ITCH", "Xmp.video.Technician"},
                                      {"IWMU", "Xmp.video.WatermarkURL"},
                                      {"IWRI", "Xmp.video.WrittenBy"},
                                      {"LANG", "Xmp.video.Language"},
                                      {"LOCA", "Xmp.video.LocationInfo"},
                                      {"PRT1", "Xmp.video.Part"},
                                      {"PRT2", "Xmp.video.NumOfParts"},
                                      {"RATE", "Xmp.video.Rate"},
                                      {"STAR", "Xmp.video.Starring"},
                                      {"STAT", "Xmp.video.Statistics"},
                                      {"TAPE", "Xmp.video.TapeName"},
                                      {"TCDO", "Xmp.video.EndTimecode"},
                                      {"TCOD", "Xmp.video.StartTimecode"},
                                      {"TITL", "Xmp.video.Title"},
                                      {"TLEN", "Xmp.video.Length"},
                                      {"TORG", "Xmp.video.Organization"},
                                      {"TRCK", "Xmp.video.TrackNumber"},
                                      {"TURL", "Xmp.video.URL"},
                                      {"TVER", "Xmp.video.SoftwareVersion"},
                                      {"VMAJ", "Xmp.video.VegasVersionMajor"},
                                      {"VMIN", "Xmp.video.VegasVersionMinor"},
                                      {"YEAR", "Xmp.video.Year"}};

constexpr TagDetails audioEncodingValues[] = {
    {0x1, "Microsoft PCM"},
    {0x2, "Microsoft ADPCM"},
    {0x3, "Microsoft IEEE float"},
    {0x4, "Compaq VSELP"},
    {0x5, "IBM CVSD"},
    {0x6, "Microsoft a-Law"},
    {0x7, "Microsoft u-Law"},
    {0x8, "Microsoft DTS"},
    {0x9, "DRM"},
    {0xa, "WMA 9 Speech"},
    {0xb, "Microsoft Windows Media RT Voice"},
    {0x10, "OKI-ADPCM"},
    {0x11, "Intel IMA/DVI-ADPCM"},
    {0x12, "Videologic Mediaspace ADPCM"},
    {0x13, "Sierra ADPCM"},
    {0x14, "Antex G.723 ADPCM"},
    {0x15, "DSP Solutions DIGISTD"},
    {0x16, "DSP Solutions DIGIFIX"},
    {0x17, "Dialoic OKI ADPCM"},
    {0x18, "Media Vision ADPCM"},
    {0x19, "HP CU"},
    {0x1a, "HP Dynamic Voice"},
    {0x20, "Yamaha ADPCM"},
    {0x21, "SONARC Speech Compression"},
    {0x22, "DSP Group True Speech"},
    {0x23, "Echo Speech Corp."},
    {0x24, "Virtual Music Audiofile AF36"},
    {0x25, "Audio Processing Tech."},
    {0x26, "Virtual Music Audiofile AF10"},
    {0x27, "Aculab Prosody 1612"},
    {0x28, "Merging Tech. LRC"},
    {0x30, "Dolby AC2"},
    {0x31, "Microsoft GSM610"},
    {0x32, "MSN Audio"},
    {0x33, "Antex ADPCME"},
    {0x34, "Control Resources VQLPC"},
    {0x35, "DSP Solutions DIGIREAL"},
    {0x36, "DSP Solutions DIGIADPCM"},
    {0x37, "Control Resources CR10"},
    {0x38, "Natural MicroSystems VBX ADPCM"},
    {0x39, "Crystal Semiconductor IMA ADPCM"},
    {0x3a, "Echo Speech ECHOSC3"},
    {0x3b, "Rockwell ADPCM"},
    {0x3c, "Rockwell DIGITALK"},
    {0x3d, "Xebec Multimedia"},
    {0x40, "Antex G.721 ADPCM"},
    {0x41, "Antex G.728 CELP"},
    {0x42, "Microsoft MSG723"},
    {0x43, "IBM AVC ADPCM"},
    {0x45, "ITU-T G.726"},
    {0x50, "Microsoft MPEG"},
    {0x51, "RT23 or PAC"},
    {0x52, "InSoft RT24"},
    {0x53, "InSoft PAC"},
    {0x55, "MP3"},
    {0x59, "Cirrus"},
    {0x60, "Cirrus Logic"},
    {0x61, "ESS Tech. PCM"},
    {0x62, "Voxware Inc."},
    {0x63, "Canopus ATRAC"},
    {0x64, "APICOM G.726 ADPCM"},
    {0x65, "APICOM G.722 ADPCM"},
    {0x66, "Microsoft DSAT"},
    {0x67, "Micorsoft DSAT DISPLAY"},
    {0x69, "Voxware Byte Aligned"},
    {0x70, "Voxware AC8"},
    {0x71, "Voxware AC10"},
    {0x72, "Voxware AC16"},
    {0x73, "Voxware AC20"},
    {0x74, "Voxware MetaVoice"},
    {0x75, "Voxware MetaSound"},
    {0x76, "Voxware RT29HW"},
    {0x77, "Voxware VR12"},
    {0x78, "Voxware VR18"},
    {0x79, "Voxware TQ40"},
    {0x7a, "Voxware SC3"},
    {0x7b, "Voxware SC3"},
    {0x80, "Soundsoft"},
    {0x81, "Voxware TQ60"},
    {0x82, "Microsoft MSRT24"},
    {0x83, "AT&T G.729A"},
    {0x84, "Motion Pixels MVI MV12"},
    {0x85, "DataFusion G.726"},
    {0x86, "DataFusion GSM610"},
    {0x88, "Iterated Systems Audio"},
    {0x89, "Onlive"},
    {0x8a, "Multitude, Inc. FT SX20"},
    {0x8b, "Infocom ITS A/S G.721 ADPCM"},
    {0x8c, "Convedia G729"},
    {0x8d, "Not specified congruency, Inc."},
    {0x91, "Siemens SBC24"},
    {0x92, "Sonic Foundry Dolby AC3 APDIF"},
    {0x93, "MediaSonic G.723"},
    {0x94, "Aculab Prosody 8kbps"},
    {0x97, "ZyXEL ADPCM"},
    {0x98, "Philips LPCBB"},
    {0x99, "Studer Professional Audio Packed"},
    {0xa0, "Malden PhonyTalk"},
    {0xa1, "Racal Recorder GSM"},
    {0xa2, "Racal Recorder G720.a"},
    {0xa3, "Racal G723.1"},
    {0xa4, "Racal Tetra ACELP"},
    {0xb0, "NEC AAC NEC Corporation"},
    {0xff, "AAC"},
    {0x100, "Rhetorex ADPCM"},
    {0x101, "IBM u-Law"},
    {0x102, "IBM a-Law"},
    {0x103, "IBM ADPCM"},
    {0x111, "Vivo G.723"},
    {0x112, "Vivo Siren"},
    {0x120, "Philips Speech Processing CELP"},
    {0x121, "Philips Speech Processing GRUNDIG"},
    {0x123, "Digital G.723"},
    {0x125, "Sanyo LD ADPCM"},
    {0x130, "Sipro Lab ACEPLNET"},
    {0x131, "Sipro Lab ACELP4800"},
    {0x132, "Sipro Lab ACELP8V3"},
    {0x133, "Sipro Lab G.729"},
    {0x134, "Sipro Lab G.729A"},
    {0x135, "Sipro Lab Kelvin"},
    {0x136, "VoiceAge AMR"},
    {0x140, "Dictaphone G.726 ADPCM"},
    {0x150, "Qualcomm PureVoice"},
    {0x151, "Qualcomm HalfRate"},
    {0x155, "Ring Zero Systems TUBGSM"},
    {0x160, "Microsoft Audio1"},
    {0x161, "Windows Media Audio V2 V7 V8 V9 / DivX audio (WMA) / Alex AC3 Audio"},
    {0x162, "Windows Media Audio Professional V9"},
    {0x163, "Windows Media Audio Lossless V9"},
    {0x164, "WMA Pro over S/PDIF"},
    {0x170, "UNISYS NAP ADPCM"},
    {0x171, "UNISYS NAP ULAW"},
    {0x172, "UNISYS NAP ALAW"},
    {0x173, "UNISYS NAP 16K"},
    {0x174, "MM SYCOM ACM SYC008 SyCom Technologies"},
    {0x175, "MM SYCOM ACM SYC701 G726L SyCom Technologies"},
    {0x176, "MM SYCOM ACM SYC701 CELP54 SyCom Technologies"},
    {0x177, "MM SYCOM ACM SYC701 CELP68 SyCom Technologies"},
    {0x178, "Knowledge Adventure ADPCM"},
    {0x180, "Fraunhofer IIS MPEG2AAC"},
    {0x190, "Digital Theater Systems DTS DS"},
    {0x200, "Creative Labs ADPCM"},
    {0x202, "Creative Labs FASTSPEECH8"},
    {0x203, "Creative Labs FASTSPEECH10"},
    {0x210, "UHER ADPCM"},
    {0x215, "Ulead DV ACM"},
    {0x216, "Ulead DV ACM"},
    {0x220, "Quarterdeck Corp."},
    {0x230, "I-Link VC"},
    {0x240, "Aureal Semiconductor Raw Sport"},
    {0x241, "ESST AC3"},
    {0x250, "Interactive Products HSX"},
    {0x251, "Interactive Products RPELP"},
    {0x260, "Consistent CS2"},
    {0x270, "Sony SCX"},
    {0x271, "Sony SCY"},
    {0x272, "Sony ATRAC3"},
    {0x273, "Sony SPC"},
    {0x280, "TELUM Telum Inc."},
    {0x281, "TELUMIA Telum Inc."},
    {0x285, "Norcom Voice Systems ADPCM"},
    {0x300, "Fujitsu FM TOWNS SND"},
    {0x301, "Fujitsu (not specified)"},
    {0x302, "Fujitsu (not specified)"},
    {0x303, "Fujitsu (not specified)"},
    {0x304, "Fujitsu (not specified)"},
    {0x305, "Fujitsu (not specified)"},
    {0x306, "Fujitsu (not specified)"},
    {0x307, "Fujitsu (not specified)"},
    {0x308, "Fujitsu (not specified)"},
    {0x350, "Micronas Semiconductors, Inc. Development"},
    {0x351, "Micronas Semiconductors, Inc. CELP833"},
    {0x400, "Brooktree Digital"},
    {0x401, "Intel Music Coder (IMC)"},
    {0x402, "Ligos Indeo Audio"},
    {0x450, "QDesign Music"},
    {0x500, "On2 VP7 On2 Technologies"},
    {0x501, "On2 VP6 On2 Technologies"},
    {0x680, "AT&T VME VMPCM"},
    {0x681, "AT&T TCP"},
    {0x700, "YMPEG Alpha (dummy for MPEG-2 compressor)"},
    {0x8ae, "ClearJump LiteWave (lossless)"},
    {0x1000, "Olivetti GSM"},
    {0x1001, "Olivetti ADPCM"},
    {0x1002, "Olivetti CELP"},
    {0x1003, "Olivetti SBC"},
    {0x1004, "Olivetti OPR"},
    {0x1100, "Lernout & Hauspie"},
    {0x1101, "Lernout & Hauspie CELP codec"},
    {0x1102, "Lernout & Hauspie SBC codec"},
    {0x1103, "Lernout & Hauspie SBC codec"},
    {0x1104, "Lernout & Hauspie SBC codec"},
    {0x1400, "Norris Comm. Inc."},
    {0x1401, "ISIAudio"},
    {0x1500, "AT&T Soundspace Music Compression"},
    {0x181c, "VoxWare RT24 speech codec"},
    {0x181e, "Lucent elemedia AX24000P Music codec"},
    {0x1971, "Sonic Foundry LOSSLESS"},
    {0x1979, "Innings Telecom Inc. ADPCM"},
    {0x1c07, "Lucent SX8300P speech codec"},
    {0x1c0c, "Lucent SX5363S G.723 compliant codec"},
    {0x1f03, "CUseeMe DigiTalk (ex-Rocwell)"},
    {0x1fc4, "NCT Soft ALF2CD ACM"},
    {0x2000, "FAST Multimedia DVM"},
    {0x2001, "Dolby DTS (Digital Theater System)"},
    {0x2002, "RealAudio 1 / 2 14.4"},
    {0x2003, "RealAudio 1 / 2 28.8"},
    {0x2004, "RealAudio G2 / 8 Cook (low bitrate)"},
    {0x2005, "RealAudio 3 / 4 / 5 Music (DNET)"},
    {0x2006, "RealAudio 10 AAC (RAAC)"},
    {0x2007, "RealAudio 10 AAC+ (RACP)"},
    {0x2500, "Reserved range to 0x2600 Microsoft"},
    {0x3313, "makeAVIS (ffvfw fake AVI sound from AviSynth scripts)"},
    {0x4143, "Divio MPEG-4 AAC audio"},
    {0x4201, "Nokia adaptive multirate"},
    {0x4243, "Divio G726 Divio, Inc."},
    {0x434c, "LEAD Speech"},
    {0x564c, "LEAD Vorbis"},
    {0x5756, "WavPack Audio"},
    {0x674f, "Ogg Vorbis (mode 1)"},
    {0x6750, "Ogg Vorbis (mode 2)"},
    {0x6751, "Ogg Vorbis (mode 3)"},
    {0x676f, "Ogg Vorbis (mode 1+)"},
    {0x6770, "Ogg Vorbis (mode 2+)"},
    {0x6771, "Ogg Vorbis (mode 3+)"},
    {0x7000, "3COM NBX 3Com Corporation"},
    {0x706d, "FAAD AAC"},
    {0x7a21, "GSM-AMR (CBR, no SID)"},
    {0x7a22, "GSM-AMR (VBR, including SID)"},
    {0xa100, "Comverse Infosys Ltd. G723 1"},
    {0xa101, "Comverse Infosys Ltd. AVQSBC"},
    {0xa102, "Comverse Infosys Ltd. OLDSBC"},
    {0xa103, "Symbol Technologies G729A"},
    {0xa104, "VoiceAge AMR WB VoiceAge Corporation"},
    {0xa105, "Ingenient Technologies Inc. G726"},
    {0xa106, "ISO/MPEG-4 advanced audio Coding"},
    {0xa107, "Encore Software Ltd G726"},
    {0xa109, "Speex ACM Codec xiph.org"},
    {0xdfac, "DebugMode SonicFoundry Vegas FrameServer ACM Codec"},
    {0xe708, "Unknown -"},
    {0xf1ac, "Free Lossless Audio Codec FLAC"},
    {0xfffe, "Extensible"},
    {0xffff, "Development"}};

constexpr TagDetails nikonAVITags[] = {{0x0003, "Xmp.video.Make"},
                                       {0x0004, "Xmp.video.Model"},
                                       {0x0005, "Xmp.video.Software"},
                                       {0x0006, "Xmp.video.Equipment"},
                                       {0x0007, "Xmp.video.Orientation"},
                                       {0x0008, "Xmp.video.ExposureTime"},
                                       {0x0009, "Xmp.video.FNumber"},
                                       {0x000a, "Xmp.video.ExposureCompensation"},
                                       {0x000b, "Xmp.video.MaxApertureValue"},
                                       {0x000c, "Xmp.video.MeteringMode"},
                                       {0x000f, "Xmp.video.FocalLength"},
                                       {0x0010, "Xmp.video.XResolution"},
                                       {0x0011, "Xmp.video.YResolution"},
                                       {0x0012, "Xmp.video.ResolutionUnit"},
                                       {0x0013, "Xmp.video.DateTimeOriginal"},
                                       {0x0014, "Xmp.video.DateTimeDigitized"},
                                       {0x0016, "Xmp.video.duration"},
                                       {0x0018, "Xmp.video.FocusMode"},
                                       {0x001b, "Xmp.video.DigitalZoomRatio"},
                                       {0x001d, "Xmp.video.ColorMode"},
                                       {0x001e, "Xmp.video.Sharpness"},
                                       {0x001f, "Xmp.video.WhiteBalance"},
                                       {0x0020, "Xmp.video.ColorNoiseReduction"}};

enum streamTypeInfo { Audio = 1, MIDI, Text, Video };

}  // namespace Exiv2::Internal

namespace Exiv2 {
using namespace Exiv2::Internal;

RiffVideo::RiffVideo(BasicIo::UniquePtr io) : Image(ImageType::riff, mdNone, std::move(io)) {
}  // RiffVideo::RiffVideo

std::string RiffVideo::mimeType() const {
  return "video/riff";
}

/*!
 @brief Function used to check equality of a Tags with a
 particular string (ignores case while comparing).
 @param buf Data buffer that will contain Tag to compare
 @param str char* Pointer to string
 @return Returns true if the buffer value is equal to string.
 */
bool RiffVideo::equalsRiffTag(Exiv2::DataBuf& buf, const char* str) {
  for (size_t i = 0; i < RIFF_TAG_SIZE; i++)
    if (toupper(buf.data()[i]) != str[i])
      return false;
  return true;
}

void RiffVideo::printStructure(std::ostream& out, PrintStructureOption option, size_t depth) {
  if (io_->open() != 0) {
    throw Error(ErrorCode::kerDataSourceOpenFailed, io_->path(), strError());
  }
  // Ensure this is the correct image type
  if (!isRiffType(*io_, true)) {
    if (io_->error() || io_->eof())
      throw Error(ErrorCode::kerFailedToReadImageData);
    throw Error(ErrorCode::kerNotAnImage, "RIFF");
  }

  bool bPrint = option == kpsBasic || option == kpsRecursive;
  if (bPrint || option == kpsXMP || option == kpsIccProfile || option == kpsIptcErase) {
    byte data[RIFF_TAG_SIZE * 2];
    io_->read(data, RIFF_TAG_SIZE * 2);
    uint64_t filesize = Exiv2::getULong(data + RIFF_TAG_SIZE, littleEndian);
    DataBuf chunkId(5);

    if (bPrint) {
      out << Internal::indent(depth) << "STRUCTURE OF RIFF FILE: " << io().path() << std::endl;
      out << Internal::indent(depth) << Internal::stringFormat(" Chunk |       Length |       Offset | Payload")
          << std::endl;
    }

    const uint64_t bufMaxSize = 200;
    io_->seek(0, BasicIo::beg);  // rewind
    while (!io_->eof() && static_cast<uint64_t>(io_->tell()) < filesize) {
      auto offset = static_cast<uint64_t>(io_->tell());
      byte size_buff[RIFF_TAG_SIZE];
      io_->read(chunkId.data(), RIFF_TAG_SIZE);
      io_->read(size_buff, RIFF_TAG_SIZE);
      uint32_t size = Exiv2::getULong(size_buff, littleEndian);
      if (size > bufMaxSize) {
        io_->seek(size, BasicIo::cur);
        continue;
      }
      DataBuf payload(offset ? size : RIFF_TAG_SIZE);  // header is different from chunks
      io_->read(payload.data(), payload.size());

      if (bPrint) {
        out << Internal::indent(depth)
            << Internal::stringFormat("  %s | %12u | %12u | ", reinterpret_cast<const char*>(chunkId.data()), size,
                                      static_cast<uint32_t>(offset))
            << Internal::binaryToString(makeSlice(payload, 0, payload.size() > 32 ? 32 : payload.size())) << std::endl;
      }

      if (equalsRiffTag(chunkId, RIFF_CHUNK_HEADER_EXIF) && option == kpsRecursive) {
        // create MemIo object with the payload, then print the structure
        MemIo p(payload.c_data(), payload.size());
        printTiffStructure(p, out, option, depth);
      }

      bool bPrintPayload = (equalsRiffTag(chunkId, RIFF_CHUNK_HEADER_XMP) && option == kpsXMP) ||
                           (equalsRiffTag(chunkId, RIFF_CHUNK_HEADER_ICCP) && option == kpsIccProfile);
      if (bPrintPayload) {
        out.write(reinterpret_cast<const char*>(payload.data()), payload.size());
      }

      if (offset && io_->tell() % 2)
        io_->seek(+1, BasicIo::cur);  // skip padding byte on sub-chunks
    }
  }
}  // RiffVideo::printStructure

void RiffVideo::writeMetadata() {
}  // RiffVideo::writeMetadata

void RiffVideo::readMetadata() {
  if (io_->open() != 0)
    throw Error(ErrorCode::kerDataSourceOpenFailed, io_->path(), strError());

  // Ensure that this is the correct image type
  if (!isRiffType(*io_, false)) {
    if (io_->error() || io_->eof())
      throw Error(ErrorCode::kerFailedToReadImageData);
    throw Error(ErrorCode::kerNotAnImage, "RIFF");
  }

  IoCloser closer(*io_);
  clearMetadata();
  continueTraversing_ = true;

  xmpData_["Xmp.video.FileSize"] = io_->size() / 1048576.;
  xmpData_["Xmp.video.FileName"] = io_->path();
  xmpData_["Xmp.video.MimeType"] = mimeType();

  DataBuf buf(RIFF_TAG_SIZE + 1);

  io_->read(buf.data(), RIFF_TAG_SIZE);
  xmpData_["Xmp.video.Container"] = buf.data();

  io_->read(buf.data(), RIFF_TAG_SIZE);
  io_->read(buf.data(), RIFF_TAG_SIZE);
  xmpData_["Xmp.video.FileType"] = buf.data();

  while (continueTraversing_)
    decodeBlock();
}  // RiffVideo::readMetadata

void RiffVideo::decodeBlock() {
  DataBuf buf(RIFF_TAG_SIZE + 1);
  DataBuf buf2(RIFF_TAG_SIZE + 1);

  io_->read(buf2.data(), RIFF_TAG_SIZE);

  if (io_->eof() || equalsRiffTag(buf2, "MOVI") || equalsRiffTag(buf2, "DATA")) {
    continueTraversing_ = false;
    return;
  }
  if (equalsRiffTag(buf2, "HDRL") || equalsRiffTag(buf2, "STRL")) {
    decodeBlock();
  } else {
    io_->read(buf.data(), RIFF_TAG_SIZE);
    size_t size = Exiv2::getULong(buf.data(), littleEndian);

    tagDecoder(buf2, size);
  }
}  // RiffVideo::decodeBlock

void RiffVideo::tagDecoder(Exiv2::DataBuf& buf, size_t size) {
  uint64_t cur_pos = io_->tell();
  static bool listFlag = false, listEnd = false;

  if (equalsRiffTag(buf, "LIST")) {
    listFlag = true;
    listEnd = false;

    while (static_cast<uint64_t>(io_->tell()) < cur_pos + size)
      decodeBlock();

    listEnd = true;
    io_->seek(cur_pos + size, BasicIo::beg);
  } else if (equalsRiffTag(buf, "JUNK") && listEnd) {
    junkHandler(size);
  } else if (equalsRiffTag(buf, "AVIH")) {
    listFlag = false;
    aviHeaderTagsHandler(size);
  } else if (equalsRiffTag(buf, "STRH")) {
    listFlag = false;
    streamHandler(size);
  } else if (equalsRiffTag(buf, "STRF") || equalsRiffTag(buf, "FMT ")) {
    listFlag = false;
    if (equalsRiffTag(buf, "FMT "))
      streamType_ = Audio;
    streamFormatHandler(size);
  } else if (equalsRiffTag(buf, "STRN")) {
    listFlag = false;
    dateTimeOriginal(size, 1);
  } else if (equalsRiffTag(buf, "STRD")) {
    listFlag = false;
    streamDataTagHandler(size);
  } else if (equalsRiffTag(buf, "IDIT")) {
    listFlag = false;
    dateTimeOriginal(size);
  } else if (equalsRiffTag(buf, "INFO")) {
    listFlag = false;
    infoTagsHandler();
  } else if (equalsRiffTag(buf, "NCDT")) {
    listFlag = false;
    nikonTagsHandler();
  } else if (equalsRiffTag(buf, "ODML")) {
    listFlag = false;
    odmlTagsHandler();
  } else if (listFlag) {
    // std::cout<<"|unprocessed|"<<buf.data();
    skipListData();
  } else {
    // std::cout<<"|unprocessed|"<<buf.data();
    io_->seek(cur_pos + size, BasicIo::beg);
  }
}  // RiffVideo::tagDecoder

void RiffVideo::streamDataTagHandler(size_t size) {
  const size_t bufMinSize = 20000;
  DataBuf buf(bufMinSize);
  uint64_t cur_pos = io_->tell();

  io_->read(buf.data(), 8);

  if (equalsRiffTag(buf, "AVIF")) {
    if (size < RIFF_TAG_SIZE) {
#ifndef SUPPRESS_WARNINGS
      EXV_ERROR << " Exif Tags found in this RIFF file are not of valid size ."
                << " Entries considered invalid. Not Processed.\n";
#endif
    } else {
      io_->read(buf.data(), size - RIFF_TAG_SIZE);

      IptcData iptcData;
      XmpData xmpData;
      DummyTiffHeader tiffHeader(littleEndian);
      TiffParserWorker::decode(exifData_, iptcData, xmpData, buf.data(), buf.size(), Tag::root,
                               TiffMapping::findDecoder, &tiffHeader);

#ifndef SUPPRESS_WARNINGS
      if (!iptcData.empty()) {
        EXV_WARNING << "Ignoring IPTC information encoded in the Exif data.\n";
      }
      if (!xmpData.empty()) {
        EXV_WARNING << "Ignoring XMP information encoded in the Exif data.\n";
      }
#endif
    }
  }
  // TODO decode CasioData and ZORA Tag
  io_->seek(cur_pos + size, BasicIo::beg);

}  // RiffVideo::streamDataTagHandler

void RiffVideo::dateTimeOriginal(size_t size, int i) {
  uint64_t cur_pos = io_->tell();
  const size_t bufMinSize = 100;
  DataBuf buf(bufMinSize);
  io_->read(buf.data(), size);
  if (!i)
    xmpData_["Xmp.video.DateUTC"] = buf.data();
  else
    xmpData_["Xmp.video.StreamName"] = buf.data();
  io_->seek(cur_pos + size, BasicIo::beg);
}  // RiffVideo::dateTimeOriginal

void RiffVideo::odmlTagsHandler() {
  const size_t bufMinSize = 100;
  DataBuf buf(bufMinSize);
  io_->seek(-12, BasicIo::cur);
  io_->read(buf.data(), RIFF_TAG_SIZE);
  size_t size = Exiv2::getULong(buf.data(), littleEndian);
  size_t size2 = size;

  uint64_t cur_pos = io_->tell();
  io_->read(buf.data(), RIFF_TAG_SIZE);
  size -= RIFF_TAG_SIZE;

  while (size > 0) {
    io_->read(buf.data(), RIFF_TAG_SIZE);
    size -= RIFF_TAG_SIZE;
    if (equalsRiffTag(buf, "DMLH")) {
      io_->read(buf.data(), RIFF_TAG_SIZE);
      size -= RIFF_TAG_SIZE;
      io_->read(buf.data(), RIFF_TAG_SIZE);
      size -= RIFF_TAG_SIZE;
      xmpData_["Xmp.video.TotalFrameCount"] = Exiv2::getULong(buf.data(), littleEndian);
    }
  }
  io_->seek(cur_pos + size2, BasicIo::beg);
}  // RiffVideo::odmlTagsHandler

void RiffVideo::skipListData() {
  DataBuf buf(RIFF_TAG_SIZE + 1);
  io_->seek(-12, BasicIo::cur);
  io_->read(buf.data(), RIFF_TAG_SIZE);
  size_t size = Exiv2::getULong(buf.data(), littleEndian);

  uint64_t cur_pos = io_->tell();
  io_->seek(cur_pos + size, BasicIo::beg);
}  // RiffVideo::skipListData

void RiffVideo::copyTagValue(DataBuf& buf_dest, DataBuf& buf_src, size_t index) {
  buf_dest.data()[0] = buf_src.data()[0 + index];
  buf_dest.data()[1] = buf_src.data()[1 + index];
  buf_dest.data()[2] = buf_src.data()[2 + index];
  buf_dest.data()[3] = buf_src.data()[3 + index];
}

void RiffVideo::nikonTagsHandler() {
  const size_t bufMinSize = 100;
  DataBuf buf(bufMinSize), buf2(RIFF_TAG_SIZE + 1);
  io_->seek(-12, BasicIo::cur);
  io_->read(buf.data(), RIFF_TAG_SIZE);

  size_t internal_size = 0, tagID = 0, dataSize = 0, tempSize, size = Exiv2::getULong(buf.data(), littleEndian);
  tempSize = size;
  char str[9] = " . . . ";
  uint64_t internal_pos, cur_pos;
  internal_pos = cur_pos = io_->tell();
  const TagDetails* td;
  double denominator = 1;
  io_->read(buf.data(), RIFF_TAG_SIZE);
  tempSize -= RIFF_TAG_SIZE;

  while (tempSize > 0) {
    std::memset(buf.data(), 0x0, buf.size());
    io_->read(buf.data(), RIFF_TAG_SIZE);
    io_->read(buf2.data(), RIFF_TAG_SIZE);
    size_t temp = internal_size = Exiv2::getULong(buf2.data(), littleEndian);
    internal_pos = io_->tell();
    tempSize -= (internal_size + 8);

    if (equalsRiffTag(buf, "NCVR")) {
      while (temp > 3) {
        std::memset(buf.data(), 0x0, buf.size());
        io_->read(buf.data(), 2);
        tagID = Exiv2::getULong(buf.data(), littleEndian);
        io_->read(buf.data(), 2);
        dataSize = Exiv2::getULong(buf.data(), littleEndian);
        temp -= (RIFF_TAG_SIZE + dataSize);

        if (tagID == 0x0001) {
          if (dataSize <= 0) {
#ifndef SUPPRESS_WARNINGS
            EXV_ERROR << " Makernotes found in this RIFF file are not of valid size ."
                      << " Entries considered invalid. Not Processed.\n";
#endif
          } else {
            io_->read(buf.data(), dataSize);
            xmpData_["Xmp.video.MakerNoteType"] = buf.data();
          }
        } else if (tagID == 0x0002) {
          while (dataSize) {
            std::memset(buf.data(), 0x0, buf.size());
            io_->read(buf.data(), 1);
            str[(RIFF_TAG_SIZE - dataSize) * 2] = static_cast<char>(Exiv2::getULong(buf.data(), littleEndian) + 48);
            --dataSize;
          }
          xmpData_["Xmp.video.MakerNoteVersion"] = str;
        }
      }
    } else if (equalsRiffTag(buf, "NCTG")) {
      while (temp > 3) {
        std::memset(buf.data(), 0x0, buf.size());
        io_->read(buf.data(), 2);
        tagID = Exiv2::getULong(buf.data(), littleEndian);
        io_->read(buf.data(), 2);
        dataSize = Exiv2::getULong(buf.data(), littleEndian);
        temp -= (RIFF_TAG_SIZE + dataSize);
        td = find(nikonAVITags, tagID);

        if (dataSize <= 0) {
#ifndef SUPPRESS_WARNINGS
          EXV_ERROR << " Makernotes found in this RIFF file are not of valid size ."
                    << " Entries considered invalid. Not Processed.\n";
#endif
        } else {
          io_->read(buf.data(), dataSize);

          switch (tagID) {
            case 0x0003:
            case 0x0004:
            case 0x0005:
            case 0x0006:
            case 0x0013:
            case 0x0014:
            case 0x0018:
            case 0x001d:
            case 0x001e:
            case 0x001f:
            case 0x0020:
              xmpData_[exvGettext(td->label_)] = buf.data();
              break;

            case 0x0007:
            case 0x0010:
            case 0x0011:
            case 0x000c:
            case 0x0012:
              xmpData_[exvGettext(td->label_)] = Exiv2::getULong(buf.data(), littleEndian);
              break;

            case 0x0008:
            case 0x0009:
            case 0x000a:
            case 0x000b:
            case 0x000f:
            case 0x001b:
            case 0x0016:
              copyTagValue(buf2, buf);
              denominator = static_cast<double>(Exiv2::getLong(buf2.data(), littleEndian));
              if (denominator != 0)
                xmpData_[exvGettext(td->label_)] = Exiv2::getLong(buf.data(), littleEndian) / denominator;
              else
                xmpData_[exvGettext(td->label_)] = 0;
              break;

            default:
              break;
          }
        }
      }
    }

    else if (equalsRiffTag(buf, "NCTH")) {  // TODO Nikon Thumbnail Image
    }

    else if (equalsRiffTag(buf, "NCVW")) {  // TODO Nikon Preview Image
    }

    io_->seek(internal_pos + internal_size, BasicIo::beg);
  }

  if (size == 0) {
    io_->seek(cur_pos + RIFF_TAG_SIZE, BasicIo::beg);
  } else {
    io_->seek(cur_pos + size, BasicIo::beg);
  }
}  // RiffVideo::nikonTagsHandler

void RiffVideo::infoTagsHandler() {
  const size_t bufMinSize = 10000;
  DataBuf buf(bufMinSize);
  io_->seek(-12, BasicIo::cur);
  io_->read(buf.data(), RIFF_TAG_SIZE);
  uint32_t infoSize, size = Exiv2::getULong(buf.data(), littleEndian);
  uint32_t size_external = size;
  const TagVocabulary* tv;

  uint64_t cur_pos = io_->tell();
  io_->read(buf.data(), RIFF_TAG_SIZE);
  size -= RIFF_TAG_SIZE;

  while (size >= RIFF_TAG_SIZE) {
    io_->read(buf.data(), RIFF_TAG_SIZE);
    size -= RIFF_TAG_SIZE;
    if (!Exiv2::getULong(buf.data(), littleEndian))
      break;
    tv = find(infoTags, Exiv2::toString(buf.data()));
    io_->read(buf.data(), RIFF_TAG_SIZE);
    size -= RIFF_TAG_SIZE;
    infoSize = Exiv2::getULong(buf.data(), littleEndian);

    size -= infoSize;
    io_->read(buf.data(), infoSize);
    if (infoSize < RIFF_TAG_SIZE)
      buf.data()[infoSize] = '\0';

    if (tv)
      xmpData_[exvGettext(tv->label_)] = buf.data();
    else
      continue;
  }
  io_->seek(cur_pos + size_external, BasicIo::beg);
}  // RiffVideo::infoTagsHandler

void RiffVideo::junkHandler(size_t size) {
  DataBuf buf(size + 1), buf2(RIFF_TAG_SIZE + 1);
  uint64_t cur_pos = io_->tell();

  io_->read(buf.data(), RIFF_TAG_SIZE);
  //! Pentax Metadata and Tags
  if (equalsRiffTag(buf, "PENT")) {
    io_->seek(cur_pos + 18, BasicIo::beg);
    io_->read(buf.data(), 26);
    xmpData_["Xmp.video.Make"] = buf.data();

    io_->read(buf.data(), 50);
    xmpData_["Xmp.video.Model"] = buf.data();

    std::memset(buf.data(), 0x0, buf.size());
    io_->read(buf.data(), 8);
    copyTagValue(buf2, buf);
    xmpData_["Xmp.video.FNumber"] =
        static_cast<double>(Exiv2::getLong(buf.data(), littleEndian)) / Exiv2::getLong(buf2.data(), littleEndian);
    ;

    io_->seek(cur_pos + 131, BasicIo::beg);
    io_->read(buf.data(), 26);
    xmpData_["Xmp.video.DateTimeOriginal"] = buf.data();

    io_->read(buf.data(), 26);
    xmpData_["Xmp.video.DateTimeDigitized"] = buf.data();

    io_->seek(cur_pos + 299, BasicIo::beg);
    std::memset(buf.data(), 0x0, buf.size());

    io_->read(buf.data(), 2);
    Exiv2::XmpTextValue tv(Exiv2::toString(Exiv2::getLong(buf.data(), littleEndian)));
    xmpData_.add(Exiv2::XmpKey("Xmp.xmp.Thumbnails/xmpGImg:width"), &tv);

    io_->read(buf.data(), 2);
    tv.read(Exiv2::toString(Exiv2::getLong(buf.data(), littleEndian)));
    xmpData_.add(Exiv2::XmpKey("Xmp.xmp.Thumbnails/xmpGImg:height"), &tv);

    io_->read(buf.data(), RIFF_TAG_SIZE);

    // TODO - Storing the image Thumbnail in Base64 Format

  } else {
    io_->seek(cur_pos, BasicIo::beg);
    io_->read(buf.data(), size);
    xmpData_["Xmp.video.Junk"] = buf.data();
  }

  io_->seek(cur_pos + size, BasicIo::beg);
}  // RiffVideo::junkHandler

void RiffVideo::aviHeaderTagsHandler(size_t size) {
  DataBuf buf(RIFF_TAG_SIZE + 1);
  size_t width = 0, height = 0, frame_count = 0;
  double frame_rate = 1;

  uint64_t cur_pos = io_->tell();

  enum aviHeaderTags { frameRate, maxDataRate, frameCount = 4, streamCount = 6, imageWidth_h = 8, imageHeight_h, last };
  for (aviHeaderTags tag = frameRate; tag != aviHeaderTags::last; tag = static_cast<aviHeaderTags>(tag + 1)) {
    std::memset(buf.data(), 0x0, buf.size());
    io_->read(buf.data(), RIFF_TAG_SIZE);

    switch (tag) {
      case frameRate:
        xmpData_["Xmp.video.MicroSecPerFrame"] = Exiv2::getULong(buf.data(), littleEndian);
        frame_rate = 1000000. / Exiv2::getULong(buf.data(), littleEndian);
        break;
      case (maxDataRate):
        xmpData_["Xmp.video.MaxDataRate"] = Exiv2::getULong(buf.data(), littleEndian) / 1024.;
        break;
      case frameCount:
        frame_count = Exiv2::getULong(buf.data(), littleEndian);
        xmpData_["Xmp.video.FrameCount"] = frame_count;
        break;
      case streamCount:
        xmpData_["Xmp.video.StreamCount"] = Exiv2::getULong(buf.data(), littleEndian);
        break;
      case imageWidth_h:
        width = Exiv2::getULong(buf.data(), littleEndian);
        xmpData_["Xmp.video.Width"] = width;
        break;
      case imageHeight_h:
        height = Exiv2::getULong(buf.data(), littleEndian);
        xmpData_["Xmp.video.Height"] = height;
        break;
      default:
        break;
    }
  }

  fillAspectRatio(width, height);
  fillDuration(frame_rate, frame_count);

  io_->seek(cur_pos + size, BasicIo::beg);
}  // RiffVideo::aviHeaderTagsHandler

void RiffVideo::streamHandler(size_t size) {
  DataBuf buf(RIFF_TAG_SIZE + 1);
  size_t divisor = 1;
  uint64_t cur_pos = io_->tell();

  io_->read(buf.data(), RIFF_TAG_SIZE);
  if (equalsRiffTag(buf, "VIDS"))
    streamType_ = Video;
  else if (equalsRiffTag(buf, "AUDS"))
    streamType_ = Audio;

  enum streamHeaderTags {
    codec = 1,
    sampleRateDivisor = 5,
    sampleRate = 6,
    sampleCount = 8,
    quality = 10,
    sampleSize = 11,
    last
  };
  for (streamHeaderTags tag = codec; tag != streamHeaderTags::last; tag = static_cast<streamHeaderTags>(tag + 1)) {
    std::memset(buf.data(), 0x0, buf.size());
    io_->read(buf.data(), RIFF_TAG_SIZE);  // the position is advanced by the number of bytes read, that's why we need
                                           // to iterate sequentially , not only on switch values.

    switch (tag) {
      case codec:
        if (streamType_ == Video)
          xmpData_["Xmp.video.Codec"] = buf.data();
        else if (streamType_ == Audio)
          xmpData_["Xmp.audio.Codec"] = buf.data();
        else
          xmpData_["Xmp.video.Codec"] = buf.data();
        break;
      case sampleRateDivisor:
        divisor = Exiv2::getULong(buf.data(), littleEndian);
        break;
      case sampleRate:
        if (streamType_ == Video)
          xmpData_["Xmp.video.FrameRate"] = returnSampleRate(buf, divisor);
        else if (streamType_ == Audio)
          xmpData_["Xmp.audio.SampleRate"] = returnSampleRate(buf, divisor);
        else
          xmpData_["Xmp.video.StreamSampleRate"] = returnSampleRate(buf, divisor);
        break;
      case sampleCount:
        if (streamType_ == Video)
          xmpData_["Xmp.video.FrameCount"] = Exiv2::getULong(buf.data(), littleEndian);
        else if (streamType_ == Audio)
          xmpData_["Xmp.audio.SampleCount"] = Exiv2::getULong(buf.data(), littleEndian);
        else
          xmpData_["Xmp.video.StreamSampleCount"] = Exiv2::getULong(buf.data(), littleEndian);
        break;
      case quality:
        if (streamType_ == Video)
          xmpData_["Xmp.video.VideoQuality"] = Exiv2::getULong(buf.data(), littleEndian);
        else if (streamType_ != Audio)
          xmpData_["Xmp.video.StreamQuality"] = Exiv2::getULong(buf.data(), littleEndian);
        break;
      case sampleSize:
        if (streamType_ == Video)
          xmpData_["Xmp.video.VideoSampleSize"] = Exiv2::getULong(buf.data(), littleEndian);
        else if (streamType_ != Audio)
          xmpData_["Xmp.video.StreamSampleSize"] = Exiv2::getULong(buf.data(), littleEndian);
        break;
      default:
        break;
    }
  }
  io_->seek(cur_pos + size, BasicIo::beg);
}  // RiffVideo::streamHandler

void RiffVideo::streamFormatHandler(size_t size) {
  DataBuf buf(RIFF_TAG_SIZE + 1);
  uint64_t cur_pos = io_->tell();

  if (streamType_ == Video) {
    io_->read(buf.data(), RIFF_TAG_SIZE);

    enum bmptags {
      imageWidth,
      imageHeight,
      planes,
      bitDepth,
      compression,
      imageLength,
      pixelsPerMeterX,
      pixelsPerMeterY,
      numColors,
      numImportantColors,
      last
    };
    for (bmptags tag = imageWidth; tag != bmptags::last; tag = static_cast<bmptags>(tag + 1)) {
      std::memset(buf.data(), 0x0, buf.size());

      switch (tag) {
        case imageWidth:  // Will be used in case of debugging
          io_->read(buf.data(), RIFF_TAG_SIZE);
          break;
        case imageHeight:  // Will be used in case of debugging
          io_->read(buf.data(), RIFF_TAG_SIZE);
          break;
        case planes:
          io_->read(buf.data(), 2);
          xmpData_["Xmp.video.Planes"] = Exiv2::getUShort(buf.data(), littleEndian);
          break;
        case bitDepth:
          io_->read(buf.data(), 2);
          xmpData_["Xmp.video.PixelDepth"] = Exiv2::getUShort(buf.data(), littleEndian);
          break;
        case compression:
          io_->read(buf.data(), RIFF_TAG_SIZE);
          xmpData_["Xmp.video.Compressor"] = buf.data();
          break;
        case imageLength:
          io_->read(buf.data(), RIFF_TAG_SIZE);
          xmpData_["Xmp.video.ImageLength"] = Exiv2::getULong(buf.data(), littleEndian);
          break;
        case pixelsPerMeterX:
          io_->read(buf.data(), RIFF_TAG_SIZE);
          xmpData_["Xmp.video.PixelPerMeterX"] = Exiv2::getULong(buf.data(), littleEndian);
          break;
        case pixelsPerMeterY:
          io_->read(buf.data(), RIFF_TAG_SIZE);
          xmpData_["Xmp.video.PixelPerMeterY"] = Exiv2::getULong(buf.data(), littleEndian);
          break;
        case numColors:
          io_->read(buf.data(), RIFF_TAG_SIZE);
          if (Exiv2::getULong(buf.data(), littleEndian) == 0) {
            xmpData_["Xmp.video.NumOfColours"] = "Unspecified";
          } else {
            xmpData_["Xmp.video.NumOfColours"] = Exiv2::getULong(buf.data(), littleEndian);
          }
          break;
        case numImportantColors:
          io_->read(buf.data(), RIFF_TAG_SIZE);
          if (Exiv2::getULong(buf.data(), littleEndian)) {
            xmpData_["Xmp.video.NumIfImpColours"] = Exiv2::getULong(buf.data(), littleEndian);
          } else {
            xmpData_["Xmp.video.NumOfImpColours"] = "All";
          }
          break;
        default:
          break;
      }
    }
  } else if (streamType_ == Audio) {
    int c = 0;
    const TagDetails* td;
    enum audioFormatTags { encoding, numberOfChannels, audioSampleRate, avgBytesPerSec = 4, bitsPerSample = 7, last };
    for (audioFormatTags tag = encoding; tag != audioFormatTags::last; tag = static_cast<audioFormatTags>(tag + 1)) {
      io_->read(buf.data(), 2);

      switch (tag) {
        case encoding:
          td = find(audioEncodingValues, Exiv2::getUShort(buf.data(), littleEndian));
          if (td) {
            xmpData_["Xmp.audio.Compressor"] = exvGettext(td->label_);
          } else {
            xmpData_["Xmp.audio.Compressor"] = Exiv2::getUShort(buf.data(), littleEndian);
          }
          break;
        case numberOfChannels:
          c = Exiv2::getUShort(buf.data(), littleEndian);
          if (c == 1)
            xmpData_["Xmp.audio.ChannelType"] = "Mono";
          else if (c == 2)
            xmpData_["Xmp.audio.ChannelType"] = "Stereo";
          else if (c == 5)
            xmpData_["Xmp.audio.ChannelType"] = "5.1 Surround Sound";
          else if (c == 7)
            xmpData_["Xmp.audio.ChannelType"] = "7.1 Surround Sound";
          else
            xmpData_["Xmp.audio.ChannelType"] = "Mono";
          break;
        case audioSampleRate:
          xmpData_["Xmp.audio.SampleRate"] = Exiv2::getUShort(buf.data(), littleEndian);
          break;
        case avgBytesPerSec:
          xmpData_["Xmp.audio.SampleType"] = Exiv2::getUShort(buf.data(), littleEndian);
          break;
        case bitsPerSample:
          xmpData_["Xmp.audio.BitsPerSample"] = Exiv2::getUShort(buf.data(), littleEndian);
          io_->read(buf.data(), 2);
          break;
        default:
          break;
      }
    }
  }
  io_->seek(cur_pos + size, BasicIo::beg);
}  // RiffVideo::streamFormatHandler

double RiffVideo::returnSampleRate(Exiv2::DataBuf& buf, size_t divisor) {
  return (static_cast<double>(Exiv2::getULong(buf.data(), littleEndian)) / divisor);
}  // RiffVideo::returnSampleRate

const char* RiffVideo::printAudioEncoding(uint64_t i) {
  const TagDetails* td;
  td = find(audioEncodingValues, i);
  if (td)
    return exvGettext(td->label_);

  return "Undefined";
}  // RiffVideo::printAudioEncoding

void RiffVideo::fillAspectRatio(size_t width, size_t height) {
  if (height == 0)
    return;
  double aspectRatio = static_cast<double>(width) / height;
  aspectRatio = floor(aspectRatio * 10) / 10;
  xmpData_["Xmp.video.AspectRatio"] = aspectRatio;

  auto aR = static_cast<int>((aspectRatio * 10.0) + 0.1);

  switch (aR) {
    case 13:
      xmpData_["Xmp.video.AspectRatio"] = "4:3";
      break;
    case 17:
      xmpData_["Xmp.video.AspectRatio"] = "16:9";
      break;
    case 10:
      xmpData_["Xmp.video.AspectRatio"] = "1:1";
      break;
    case 16:
      xmpData_["Xmp.video.AspectRatio"] = "16:10";
      break;
    case 22:
      xmpData_["Xmp.video.AspectRatio"] = "2.21:1";
      break;
    case 23:
      xmpData_["Xmp.video.AspectRatio"] = "2.35:1";
      break;
    case 12:
      xmpData_["Xmp.video.AspectRatio"] = "5:4";
      break;
    default:
      xmpData_["Xmp.video.AspectRatio"] = aspectRatio;
      break;
  }
}  // RiffVideo::fillAspectRatio

void RiffVideo::fillDuration(double frame_rate, size_t frame_count) {
  if (frame_rate == 0)
    return;

  auto duration = static_cast<uint64_t>(frame_count * 1000. / frame_rate);
  xmpData_["Xmp.video.FileDataRate"] = io_->size() / (1048576. * duration);
  xmpData_["Xmp.video.Duration"] = duration;  // Duration in number of seconds
}  // RiffVideo::fillDuration

Image::UniquePtr newRiffInstance(BasicIo::UniquePtr io, bool /*create*/) {
  auto image = std::make_unique<RiffVideo>(std::move(io));
  if (!image->good()) {
    return nullptr;
  }
  return image;
}

bool isRiffType(BasicIo& iIo, bool advance) {
  const int32_t len = 2;
  const unsigned char RiffVideoId[4] = {'R', 'I', 'F', 'F'};
  byte buf[len];
  iIo.read(buf, len);
  if (iIo.error() || iIo.eof()) {
    return false;
  }
  bool matched = (memcmp(buf, RiffVideoId, len) == 0);
  if (!advance || !matched) {
    iIo.seek(-len, BasicIo::cur);
  }
  return matched;
}
}  // namespace Exiv2
