#ifndef __IDCLASSES_HPP__
#define __IDCLASSES_HPP__


class SeriesID
{
public:
           SeriesID(){ id_ = -1; }
  explicit SeriesID(int id) : id_(id) {}

  QString ToString() const { return QString::number(id_); }
  int     ToInt() const { return id_; }
  bool    IsValid() const { return id_ >= 0; }

  bool operator==(const SeriesID& rhs) const { return id_ == rhs.id_; }
  bool operator!=(const SeriesID& rhs) const { return id_ != rhs.id_; }

private:
  int id_;
};

class ViewSessionID
{
public:
           ViewSessionID(){ id_ = -1; }
  explicit ViewSessionID(int id) : id_(id) {}

  QString ToString() const { return QString::number(id_); }
  int     ToInt() const { return id_; }
  bool    IsValid() const { return id_ >= 0; }

  bool operator==(const ViewSessionID& rhs) const { return id_ == rhs.id_; }
  bool operator!=(const ViewSessionID& rhs) const { return id_ != rhs.id_; }

private:
  int id_;
};


#endif // __IDCLASSES_HPP__

