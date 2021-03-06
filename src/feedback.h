// Copyright 2014 Toggl Desktop developers.

#ifndef SRC_FEEDBACK_H_
#define SRC_FEEDBACK_H_

#include "./feedback.h"

#include <string>

#include "./types.h"

namespace toggl {

class Feedback {
 public:
    Feedback(
        const std::string topic,
        const std::string details,
        const std::string attachment_path)
        : subject_(topic)
    , details_(details)
    , attachment_path_(attachment_path) {}
    ~Feedback() {}

    toggl::error Validate() const;
    const std::string JSON() const;

 private:
    const std::string filename() const;
    const std::string base64encode_attachment() const;

    std::string subject_;
    std::string details_;
    std::string attachment_path_;
};

}  // namespace toggl

#endif  // SRC_FEEDBACK_H_
