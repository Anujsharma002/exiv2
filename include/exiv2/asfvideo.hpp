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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301 USA.
 */
/*!
  @file    asfvideo.hpp
  @brief   An Image subclass to support ASF video files
  @author  Abhinav Badola for GSoC 2012
           <a href="mailto:mail.abu.to@gmail.com">mail.abu.to@gmail.com</a>
  @date    08-Aug-12, AB: created
 */
#ifndef ASFVIDEO_HPP
#define ASFVIDEO_HPP

// *****************************************************************************
#include "exiv2lib_export.h"

// included header files
#include "image.hpp"

namespace Exiv2 {

// *****************************************************************************
// class definitions
/*!
  @brief Class to access ASF video files.
 */
class EXIV2API AsfVideo : public Image {
 public:
  //! @name Creators
  //@{
  /*!
    @brief Constructor for a ASF video. Since the constructor
        can not return a result, callers should check the good() method
        after object construction to determine success or failure.
    @param io An auto-pointer that owns a BasicIo instance used for
        reading and writing image metadata. \b Important: The constructor
        takes ownership of the passed in BasicIo instance through the
        auto-pointer. Callers should not continue to use the BasicIo
        instance after it is passed to this method. Use the Image::io()
        method to get a temporary reference.
   */
  explicit AsfVideo(BasicIo::UniquePtr io);

  //! Copy constructor
  AsfVideo(const AsfVideo& rhs) = delete;
  //! Assignment operator
  AsfVideo& operator=(const AsfVideo& rhs) = delete;
  //@}

  //! @name Manipulators
  //@{
  void readMetadata() override;
  void writeMetadata() override;
  //@}

  //! @name Accessors
  //@{
  std::string mimeType() const override;
  //@}
 private:
  static constexpr size_t ASF_TAG_SIZE = 0x4;
  static constexpr size_t BUFF_MIN_SIZE = 0x8;
  static constexpr size_t GUI_SIZE = 16;
  static constexpr size_t GUID_SIZE = 37;

 protected:
  /*!
    @brief Check for a valid tag and decode the block at the current IO
    position. Calls tagDecoder() or skips to next tag, if required.
   */
  void decodeBlock();
  /*!
    @brief Interpret tag information, and call the respective function
        to save it in the respective XMP container. Decodes a Tag
        Information and saves it in the respective XMP container, if
        the block size is small.
    @param tv Pointer to current tag,
    @param size Size of the data block used to store Tag Information.
   */
  void tagDecoder(uint64_t size);
  /*!
    @brief Interpret File_Properties tag information, and save it in
        the respective XMP container.
   */
  void fileProperties();
  /*!
    @brief Interpret Stream_Properties tag information, and save it
        in the respective XMP container.
   */
  void streamProperties();
  /*!
    @brief Interpret Codec_List tag information, and save it in
        the respective XMP container.
   */
  void codecList();
  /*!
    @brief Interpret Content_Description tag information, and save it
        in the respective XMP container.
    @param size Size of the data block used to store Tag Data.
   */
  void contentDescription(uint64_t size);
  /*!
    @brief Interpret Extended_Stream_Properties tag information, and
        save it in the respective XMP container.
    @param size Size of the data block used to store Tag Data.
   */
  void extendedStreamProperties(uint64_t size);
  /*!
    @brief Interpret Header_Extension tag information, and save it in
        the respective XMP container.
    @param size Size of the data block used to store Tag Data.
   */
  void headerExtension(uint64_t size);
  /*!
    @brief Interpret Metadata, Extended_Content_Description,
        Metadata_Library tag information, and save it in the respective
        XMP container.
    @param meta A default integer which helps to overload the function
        for various Tags that have a similar method of decoding.
   */
  void metadataHandler(int meta = 1);
  /*!
    @brief Calculates Aspect Ratio of a video, and stores it in the
        respective XMP container.
   */
  void aspectRatio();

 private:
  //! Variable to check the end of metadata traversing.
  bool continueTraversing_;
  //! Variable which stores current position of the read pointer.
  uint64_t localPosition_;
  //! Variable which stores current stream being processsed.
  int streamNumber_;
  //! Variable to store height and width of a video frame.
  uint64_t height_, width_;

};  // Class AsfVideo

// *****************************************************************************
// template, inline and free functions

// These could be static private functions on Image subclasses but then
// ImageFactory needs to be made a friend.
/*!
  @brief Create a new AsfVideo instance and return an auto-pointer to it.
      Caller owns the returned object and the auto-pointer ensures that
      it will be deleted.
 */
EXIV2API Image::UniquePtr newAsfInstance(BasicIo::UniquePtr io, bool create);

//! Check if the file iIo is a Windows Asf Video.
EXIV2API bool isAsfType(BasicIo& iIo, bool advance);

}  // namespace Exiv2

#endif  // #ifndef ASFVIDEO_HPP_