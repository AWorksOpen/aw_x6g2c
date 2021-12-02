#if defined(LINUX) || defined(WIN32)
#include "diff_image_to_video.h"
#include "diff_image_to_video_save_image_data.h"

#ifdef WIN32

#include <Windows.h>

#else

#endif

using namespace std;

namespace diff_image_video {
diff_image_to_video::diff_image_to_video(){};
diff_image_to_video::~diff_image_to_video(){};

bool diff_image_to_video::save_image_data_list(const string &image_dir,
                                               const string &image_name_format,
                                               const string &save_file_path,
                                               image_format_t frame_image_type,
                                               unsigned int delays) {
  diff_image_to_video_save_image_data save_handle;
  return save_handle.save_image_data_list(
      image_dir, image_name_format, save_file_path, frame_image_type, delays);
}

bool diff_image_to_video::save_image_data_list(const list<string> &file_list,
                                               const string &image_name_format,
                                               const string &save_file_path,
                                               image_format_t frame_image_type,
                                               unsigned int delays) {
  diff_image_to_video_save_image_data save_handle;
  return save_handle.save_image_data_list(
      file_list, image_name_format, save_file_path, frame_image_type, delays);
}

}; // namespace diff_image_video

#endif
