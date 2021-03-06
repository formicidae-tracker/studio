// Code generated by protoc-gen-go. DO NOT EDIT.
// versions:
// 	protoc-gen-go v1.24.0-devel
// 	protoc        v3.6.1
// source: TrackingDataDirectory.proto

package fort_myrmidon_pb

import (
	proto "github.com/golang/protobuf/proto"
	protoreflect "google.golang.org/protobuf/reflect/protoreflect"
	protoimpl "google.golang.org/protobuf/runtime/protoimpl"
	reflect "reflect"
	sync "sync"
)

const (
	// Verify that this generated code is sufficiently up-to-date.
	_ = protoimpl.EnforceVersion(20 - protoimpl.MinVersion)
	// Verify that runtime/protoimpl is sufficiently up-to-date.
	_ = protoimpl.EnforceVersion(protoimpl.MaxVersion - 20)
)

// This is a compile-time assertion that a sufficiently up-to-date version
// of the legacy proto package is being used.
const _ = proto.ProtoPackageIsVersion4

type TimedFrame struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Frameid uint64 `protobuf:"varint,1,opt,name=frameid,proto3" json:"frameid,omitempty"`
	Time    *Time  `protobuf:"bytes,2,opt,name=time,proto3" json:"time,omitempty"`
}

func (x *TimedFrame) Reset() {
	*x = TimedFrame{}
	if protoimpl.UnsafeEnabled {
		mi := &file_TrackingDataDirectory_proto_msgTypes[0]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *TimedFrame) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*TimedFrame) ProtoMessage() {}

func (x *TimedFrame) ProtoReflect() protoreflect.Message {
	mi := &file_TrackingDataDirectory_proto_msgTypes[0]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use TimedFrame.ProtoReflect.Descriptor instead.
func (*TimedFrame) Descriptor() ([]byte, []int) {
	return file_TrackingDataDirectory_proto_rawDescGZIP(), []int{0}
}

func (x *TimedFrame) GetFrameid() uint64 {
	if x != nil {
		return x.Frameid
	}
	return 0
}

func (x *TimedFrame) GetTime() *Time {
	if x != nil {
		return x.Time
	}
	return nil
}

type TrackingSegment struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Frame    *TimedFrame `protobuf:"bytes,1,opt,name=frame,proto3" json:"frame,omitempty"`
	Filename string      `protobuf:"bytes,2,opt,name=filename,proto3" json:"filename,omitempty"`
}

func (x *TrackingSegment) Reset() {
	*x = TrackingSegment{}
	if protoimpl.UnsafeEnabled {
		mi := &file_TrackingDataDirectory_proto_msgTypes[1]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *TrackingSegment) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*TrackingSegment) ProtoMessage() {}

func (x *TrackingSegment) ProtoReflect() protoreflect.Message {
	mi := &file_TrackingDataDirectory_proto_msgTypes[1]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use TrackingSegment.ProtoReflect.Descriptor instead.
func (*TrackingSegment) Descriptor() ([]byte, []int) {
	return file_TrackingDataDirectory_proto_rawDescGZIP(), []int{1}
}

func (x *TrackingSegment) GetFrame() *TimedFrame {
	if x != nil {
		return x.Frame
	}
	return nil
}

func (x *TrackingSegment) GetFilename() string {
	if x != nil {
		return x.Filename
	}
	return ""
}

type MovieOffset struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	MovieFrameID uint64 `protobuf:"varint,1,opt,name=movieFrameID,proto3" json:"movieFrameID,omitempty"`
	Offset       uint64 `protobuf:"varint,2,opt,name=offset,proto3" json:"offset,omitempty"`
}

func (x *MovieOffset) Reset() {
	*x = MovieOffset{}
	if protoimpl.UnsafeEnabled {
		mi := &file_TrackingDataDirectory_proto_msgTypes[2]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *MovieOffset) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*MovieOffset) ProtoMessage() {}

func (x *MovieOffset) ProtoReflect() protoreflect.Message {
	mi := &file_TrackingDataDirectory_proto_msgTypes[2]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use MovieOffset.ProtoReflect.Descriptor instead.
func (*MovieOffset) Descriptor() ([]byte, []int) {
	return file_TrackingDataDirectory_proto_rawDescGZIP(), []int{2}
}

func (x *MovieOffset) GetMovieFrameID() uint64 {
	if x != nil {
		return x.MovieFrameID
	}
	return 0
}

func (x *MovieOffset) GetOffset() uint64 {
	if x != nil {
		return x.Offset
	}
	return 0
}

type MovieSegment struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Id            uint32         `protobuf:"varint,1,opt,name=id,proto3" json:"id,omitempty"`
	Path          string         `protobuf:"bytes,2,opt,name=path,proto3" json:"path,omitempty"`
	TrackingStart uint64         `protobuf:"varint,3,opt,name=trackingStart,proto3" json:"trackingStart,omitempty"`
	TrackingEnd   uint64         `protobuf:"varint,4,opt,name=trackingEnd,proto3" json:"trackingEnd,omitempty"`
	MovieStart    uint64         `protobuf:"varint,5,opt,name=movieStart,proto3" json:"movieStart,omitempty"`
	MovieEnd      uint64         `protobuf:"varint,6,opt,name=movieEnd,proto3" json:"movieEnd,omitempty"`
	Offsets       []*MovieOffset `protobuf:"bytes,8,rep,name=offsets,proto3" json:"offsets,omitempty"`
}

func (x *MovieSegment) Reset() {
	*x = MovieSegment{}
	if protoimpl.UnsafeEnabled {
		mi := &file_TrackingDataDirectory_proto_msgTypes[3]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *MovieSegment) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*MovieSegment) ProtoMessage() {}

func (x *MovieSegment) ProtoReflect() protoreflect.Message {
	mi := &file_TrackingDataDirectory_proto_msgTypes[3]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use MovieSegment.ProtoReflect.Descriptor instead.
func (*MovieSegment) Descriptor() ([]byte, []int) {
	return file_TrackingDataDirectory_proto_rawDescGZIP(), []int{3}
}

func (x *MovieSegment) GetId() uint32 {
	if x != nil {
		return x.Id
	}
	return 0
}

func (x *MovieSegment) GetPath() string {
	if x != nil {
		return x.Path
	}
	return ""
}

func (x *MovieSegment) GetTrackingStart() uint64 {
	if x != nil {
		return x.TrackingStart
	}
	return 0
}

func (x *MovieSegment) GetTrackingEnd() uint64 {
	if x != nil {
		return x.TrackingEnd
	}
	return 0
}

func (x *MovieSegment) GetMovieStart() uint64 {
	if x != nil {
		return x.MovieStart
	}
	return 0
}

func (x *MovieSegment) GetMovieEnd() uint64 {
	if x != nil {
		return x.MovieEnd
	}
	return 0
}

func (x *MovieSegment) GetOffsets() []*MovieOffset {
	if x != nil {
		return x.Offsets
	}
	return nil
}

type TrackingDataDirectory struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Start        *TimedFrame `protobuf:"bytes,1,opt,name=Start,proto3" json:"Start,omitempty"`
	End          *TimedFrame `protobuf:"bytes,2,opt,name=End,proto3" json:"End,omitempty"`
	CacheVersion uint32      `protobuf:"varint,3,opt,name=CacheVersion,proto3" json:"CacheVersion,omitempty"`
}

func (x *TrackingDataDirectory) Reset() {
	*x = TrackingDataDirectory{}
	if protoimpl.UnsafeEnabled {
		mi := &file_TrackingDataDirectory_proto_msgTypes[4]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *TrackingDataDirectory) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*TrackingDataDirectory) ProtoMessage() {}

func (x *TrackingDataDirectory) ProtoReflect() protoreflect.Message {
	mi := &file_TrackingDataDirectory_proto_msgTypes[4]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use TrackingDataDirectory.ProtoReflect.Descriptor instead.
func (*TrackingDataDirectory) Descriptor() ([]byte, []int) {
	return file_TrackingDataDirectory_proto_rawDescGZIP(), []int{4}
}

func (x *TrackingDataDirectory) GetStart() *TimedFrame {
	if x != nil {
		return x.Start
	}
	return nil
}

func (x *TrackingDataDirectory) GetEnd() *TimedFrame {
	if x != nil {
		return x.End
	}
	return nil
}

func (x *TrackingDataDirectory) GetCacheVersion() uint32 {
	if x != nil {
		return x.CacheVersion
	}
	return 0
}

type TrackingDataDirectoryFileLine struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Movie          *MovieSegment    `protobuf:"bytes,1,opt,name=movie,proto3" json:"movie,omitempty"`
	Segment        *TrackingSegment `protobuf:"bytes,2,opt,name=segment,proto3" json:"segment,omitempty"`
	CachedFrame    *TimedFrame      `protobuf:"bytes,3,opt,name=cachedFrame,proto3" json:"cachedFrame,omitempty"`
	MovieStartTime *Time            `protobuf:"bytes,4,opt,name=movieStartTime,proto3" json:"movieStartTime,omitempty"`
}

func (x *TrackingDataDirectoryFileLine) Reset() {
	*x = TrackingDataDirectoryFileLine{}
	if protoimpl.UnsafeEnabled {
		mi := &file_TrackingDataDirectory_proto_msgTypes[5]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *TrackingDataDirectoryFileLine) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*TrackingDataDirectoryFileLine) ProtoMessage() {}

func (x *TrackingDataDirectoryFileLine) ProtoReflect() protoreflect.Message {
	mi := &file_TrackingDataDirectory_proto_msgTypes[5]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use TrackingDataDirectoryFileLine.ProtoReflect.Descriptor instead.
func (*TrackingDataDirectoryFileLine) Descriptor() ([]byte, []int) {
	return file_TrackingDataDirectory_proto_rawDescGZIP(), []int{5}
}

func (x *TrackingDataDirectoryFileLine) GetMovie() *MovieSegment {
	if x != nil {
		return x.Movie
	}
	return nil
}

func (x *TrackingDataDirectoryFileLine) GetSegment() *TrackingSegment {
	if x != nil {
		return x.Segment
	}
	return nil
}

func (x *TrackingDataDirectoryFileLine) GetCachedFrame() *TimedFrame {
	if x != nil {
		return x.CachedFrame
	}
	return nil
}

func (x *TrackingDataDirectoryFileLine) GetMovieStartTime() *Time {
	if x != nil {
		return x.MovieStartTime
	}
	return nil
}

var File_TrackingDataDirectory_proto protoreflect.FileDescriptor

var file_TrackingDataDirectory_proto_rawDesc = []byte{
	0x0a, 0x1b, 0x54, 0x72, 0x61, 0x63, 0x6b, 0x69, 0x6e, 0x67, 0x44, 0x61, 0x74, 0x61, 0x44, 0x69,
	0x72, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x79, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x12, 0x10, 0x66,
	0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x1a,
	0x0a, 0x54, 0x69, 0x6d, 0x65, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x22, 0x52, 0x0a, 0x0a, 0x54,
	0x69, 0x6d, 0x65, 0x64, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x12, 0x18, 0x0a, 0x07, 0x66, 0x72, 0x61,
	0x6d, 0x65, 0x69, 0x64, 0x18, 0x01, 0x20, 0x01, 0x28, 0x04, 0x52, 0x07, 0x66, 0x72, 0x61, 0x6d,
	0x65, 0x69, 0x64, 0x12, 0x2a, 0x0a, 0x04, 0x74, 0x69, 0x6d, 0x65, 0x18, 0x02, 0x20, 0x01, 0x28,
	0x0b, 0x32, 0x16, 0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f,
	0x6e, 0x2e, 0x70, 0x62, 0x2e, 0x54, 0x69, 0x6d, 0x65, 0x52, 0x04, 0x74, 0x69, 0x6d, 0x65, 0x22,
	0x61, 0x0a, 0x0f, 0x54, 0x72, 0x61, 0x63, 0x6b, 0x69, 0x6e, 0x67, 0x53, 0x65, 0x67, 0x6d, 0x65,
	0x6e, 0x74, 0x12, 0x32, 0x0a, 0x05, 0x66, 0x72, 0x61, 0x6d, 0x65, 0x18, 0x01, 0x20, 0x01, 0x28,
	0x0b, 0x32, 0x1c, 0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f,
	0x6e, 0x2e, 0x70, 0x62, 0x2e, 0x54, 0x69, 0x6d, 0x65, 0x64, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x52,
	0x05, 0x66, 0x72, 0x61, 0x6d, 0x65, 0x12, 0x1a, 0x0a, 0x08, 0x66, 0x69, 0x6c, 0x65, 0x6e, 0x61,
	0x6d, 0x65, 0x18, 0x02, 0x20, 0x01, 0x28, 0x09, 0x52, 0x08, 0x66, 0x69, 0x6c, 0x65, 0x6e, 0x61,
	0x6d, 0x65, 0x22, 0x49, 0x0a, 0x0b, 0x4d, 0x6f, 0x76, 0x69, 0x65, 0x4f, 0x66, 0x66, 0x73, 0x65,
	0x74, 0x12, 0x22, 0x0a, 0x0c, 0x6d, 0x6f, 0x76, 0x69, 0x65, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x49,
	0x44, 0x18, 0x01, 0x20, 0x01, 0x28, 0x04, 0x52, 0x0c, 0x6d, 0x6f, 0x76, 0x69, 0x65, 0x46, 0x72,
	0x61, 0x6d, 0x65, 0x49, 0x44, 0x12, 0x16, 0x0a, 0x06, 0x6f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x18,
	0x02, 0x20, 0x01, 0x28, 0x04, 0x52, 0x06, 0x6f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x22, 0xef, 0x01,
	0x0a, 0x0c, 0x4d, 0x6f, 0x76, 0x69, 0x65, 0x53, 0x65, 0x67, 0x6d, 0x65, 0x6e, 0x74, 0x12, 0x0e,
	0x0a, 0x02, 0x69, 0x64, 0x18, 0x01, 0x20, 0x01, 0x28, 0x0d, 0x52, 0x02, 0x69, 0x64, 0x12, 0x12,
	0x0a, 0x04, 0x70, 0x61, 0x74, 0x68, 0x18, 0x02, 0x20, 0x01, 0x28, 0x09, 0x52, 0x04, 0x70, 0x61,
	0x74, 0x68, 0x12, 0x24, 0x0a, 0x0d, 0x74, 0x72, 0x61, 0x63, 0x6b, 0x69, 0x6e, 0x67, 0x53, 0x74,
	0x61, 0x72, 0x74, 0x18, 0x03, 0x20, 0x01, 0x28, 0x04, 0x52, 0x0d, 0x74, 0x72, 0x61, 0x63, 0x6b,
	0x69, 0x6e, 0x67, 0x53, 0x74, 0x61, 0x72, 0x74, 0x12, 0x20, 0x0a, 0x0b, 0x74, 0x72, 0x61, 0x63,
	0x6b, 0x69, 0x6e, 0x67, 0x45, 0x6e, 0x64, 0x18, 0x04, 0x20, 0x01, 0x28, 0x04, 0x52, 0x0b, 0x74,
	0x72, 0x61, 0x63, 0x6b, 0x69, 0x6e, 0x67, 0x45, 0x6e, 0x64, 0x12, 0x1e, 0x0a, 0x0a, 0x6d, 0x6f,
	0x76, 0x69, 0x65, 0x53, 0x74, 0x61, 0x72, 0x74, 0x18, 0x05, 0x20, 0x01, 0x28, 0x04, 0x52, 0x0a,
	0x6d, 0x6f, 0x76, 0x69, 0x65, 0x53, 0x74, 0x61, 0x72, 0x74, 0x12, 0x1a, 0x0a, 0x08, 0x6d, 0x6f,
	0x76, 0x69, 0x65, 0x45, 0x6e, 0x64, 0x18, 0x06, 0x20, 0x01, 0x28, 0x04, 0x52, 0x08, 0x6d, 0x6f,
	0x76, 0x69, 0x65, 0x45, 0x6e, 0x64, 0x12, 0x37, 0x0a, 0x07, 0x6f, 0x66, 0x66, 0x73, 0x65, 0x74,
	0x73, 0x18, 0x08, 0x20, 0x03, 0x28, 0x0b, 0x32, 0x1d, 0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d,
	0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x2e, 0x4d, 0x6f, 0x76, 0x69, 0x65,
	0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x52, 0x07, 0x6f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x73, 0x22,
	0x9f, 0x01, 0x0a, 0x15, 0x54, 0x72, 0x61, 0x63, 0x6b, 0x69, 0x6e, 0x67, 0x44, 0x61, 0x74, 0x61,
	0x44, 0x69, 0x72, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x79, 0x12, 0x32, 0x0a, 0x05, 0x53, 0x74, 0x61,
	0x72, 0x74, 0x18, 0x01, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x1c, 0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e,
	0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x2e, 0x54, 0x69, 0x6d, 0x65,
	0x64, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x52, 0x05, 0x53, 0x74, 0x61, 0x72, 0x74, 0x12, 0x2e, 0x0a,
	0x03, 0x45, 0x6e, 0x64, 0x18, 0x02, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x1c, 0x2e, 0x66, 0x6f, 0x72,
	0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x2e, 0x54, 0x69,
	0x6d, 0x65, 0x64, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x52, 0x03, 0x45, 0x6e, 0x64, 0x12, 0x22, 0x0a,
	0x0c, 0x43, 0x61, 0x63, 0x68, 0x65, 0x56, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x18, 0x03, 0x20,
	0x01, 0x28, 0x0d, 0x52, 0x0c, 0x43, 0x61, 0x63, 0x68, 0x65, 0x56, 0x65, 0x72, 0x73, 0x69, 0x6f,
	0x6e, 0x22, 0x92, 0x02, 0x0a, 0x1d, 0x54, 0x72, 0x61, 0x63, 0x6b, 0x69, 0x6e, 0x67, 0x44, 0x61,
	0x74, 0x61, 0x44, 0x69, 0x72, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x79, 0x46, 0x69, 0x6c, 0x65, 0x4c,
	0x69, 0x6e, 0x65, 0x12, 0x34, 0x0a, 0x05, 0x6d, 0x6f, 0x76, 0x69, 0x65, 0x18, 0x01, 0x20, 0x01,
	0x28, 0x0b, 0x32, 0x1e, 0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64,
	0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x2e, 0x4d, 0x6f, 0x76, 0x69, 0x65, 0x53, 0x65, 0x67, 0x6d, 0x65,
	0x6e, 0x74, 0x52, 0x05, 0x6d, 0x6f, 0x76, 0x69, 0x65, 0x12, 0x3b, 0x0a, 0x07, 0x73, 0x65, 0x67,
	0x6d, 0x65, 0x6e, 0x74, 0x18, 0x02, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x21, 0x2e, 0x66, 0x6f, 0x72,
	0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x2e, 0x54, 0x72,
	0x61, 0x63, 0x6b, 0x69, 0x6e, 0x67, 0x53, 0x65, 0x67, 0x6d, 0x65, 0x6e, 0x74, 0x52, 0x07, 0x73,
	0x65, 0x67, 0x6d, 0x65, 0x6e, 0x74, 0x12, 0x3e, 0x0a, 0x0b, 0x63, 0x61, 0x63, 0x68, 0x65, 0x64,
	0x46, 0x72, 0x61, 0x6d, 0x65, 0x18, 0x03, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x1c, 0x2e, 0x66, 0x6f,
	0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x2e, 0x54,
	0x69, 0x6d, 0x65, 0x64, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x52, 0x0b, 0x63, 0x61, 0x63, 0x68, 0x65,
	0x64, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x12, 0x3e, 0x0a, 0x0e, 0x6d, 0x6f, 0x76, 0x69, 0x65, 0x53,
	0x74, 0x61, 0x72, 0x74, 0x54, 0x69, 0x6d, 0x65, 0x18, 0x04, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x16,
	0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70,
	0x62, 0x2e, 0x54, 0x69, 0x6d, 0x65, 0x52, 0x0e, 0x6d, 0x6f, 0x76, 0x69, 0x65, 0x53, 0x74, 0x61,
	0x72, 0x74, 0x54, 0x69, 0x6d, 0x65, 0x62, 0x06, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x33,
}

var (
	file_TrackingDataDirectory_proto_rawDescOnce sync.Once
	file_TrackingDataDirectory_proto_rawDescData = file_TrackingDataDirectory_proto_rawDesc
)

func file_TrackingDataDirectory_proto_rawDescGZIP() []byte {
	file_TrackingDataDirectory_proto_rawDescOnce.Do(func() {
		file_TrackingDataDirectory_proto_rawDescData = protoimpl.X.CompressGZIP(file_TrackingDataDirectory_proto_rawDescData)
	})
	return file_TrackingDataDirectory_proto_rawDescData
}

var file_TrackingDataDirectory_proto_msgTypes = make([]protoimpl.MessageInfo, 6)
var file_TrackingDataDirectory_proto_goTypes = []interface{}{
	(*TimedFrame)(nil),                    // 0: fort.myrmidon.pb.TimedFrame
	(*TrackingSegment)(nil),               // 1: fort.myrmidon.pb.TrackingSegment
	(*MovieOffset)(nil),                   // 2: fort.myrmidon.pb.MovieOffset
	(*MovieSegment)(nil),                  // 3: fort.myrmidon.pb.MovieSegment
	(*TrackingDataDirectory)(nil),         // 4: fort.myrmidon.pb.TrackingDataDirectory
	(*TrackingDataDirectoryFileLine)(nil), // 5: fort.myrmidon.pb.TrackingDataDirectoryFileLine
	(*Time)(nil),                          // 6: fort.myrmidon.pb.Time
}
var file_TrackingDataDirectory_proto_depIdxs = []int32{
	6, // 0: fort.myrmidon.pb.TimedFrame.time:type_name -> fort.myrmidon.pb.Time
	0, // 1: fort.myrmidon.pb.TrackingSegment.frame:type_name -> fort.myrmidon.pb.TimedFrame
	2, // 2: fort.myrmidon.pb.MovieSegment.offsets:type_name -> fort.myrmidon.pb.MovieOffset
	0, // 3: fort.myrmidon.pb.TrackingDataDirectory.Start:type_name -> fort.myrmidon.pb.TimedFrame
	0, // 4: fort.myrmidon.pb.TrackingDataDirectory.End:type_name -> fort.myrmidon.pb.TimedFrame
	3, // 5: fort.myrmidon.pb.TrackingDataDirectoryFileLine.movie:type_name -> fort.myrmidon.pb.MovieSegment
	1, // 6: fort.myrmidon.pb.TrackingDataDirectoryFileLine.segment:type_name -> fort.myrmidon.pb.TrackingSegment
	0, // 7: fort.myrmidon.pb.TrackingDataDirectoryFileLine.cachedFrame:type_name -> fort.myrmidon.pb.TimedFrame
	6, // 8: fort.myrmidon.pb.TrackingDataDirectoryFileLine.movieStartTime:type_name -> fort.myrmidon.pb.Time
	9, // [9:9] is the sub-list for method output_type
	9, // [9:9] is the sub-list for method input_type
	9, // [9:9] is the sub-list for extension type_name
	9, // [9:9] is the sub-list for extension extendee
	0, // [0:9] is the sub-list for field type_name
}

func init() { file_TrackingDataDirectory_proto_init() }
func file_TrackingDataDirectory_proto_init() {
	if File_TrackingDataDirectory_proto != nil {
		return
	}
	file_Time_proto_init()
	if !protoimpl.UnsafeEnabled {
		file_TrackingDataDirectory_proto_msgTypes[0].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*TimedFrame); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_TrackingDataDirectory_proto_msgTypes[1].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*TrackingSegment); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_TrackingDataDirectory_proto_msgTypes[2].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*MovieOffset); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_TrackingDataDirectory_proto_msgTypes[3].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*MovieSegment); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_TrackingDataDirectory_proto_msgTypes[4].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*TrackingDataDirectory); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_TrackingDataDirectory_proto_msgTypes[5].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*TrackingDataDirectoryFileLine); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
	}
	type x struct{}
	out := protoimpl.TypeBuilder{
		File: protoimpl.DescBuilder{
			GoPackagePath: reflect.TypeOf(x{}).PkgPath(),
			RawDescriptor: file_TrackingDataDirectory_proto_rawDesc,
			NumEnums:      0,
			NumMessages:   6,
			NumExtensions: 0,
			NumServices:   0,
		},
		GoTypes:           file_TrackingDataDirectory_proto_goTypes,
		DependencyIndexes: file_TrackingDataDirectory_proto_depIdxs,
		MessageInfos:      file_TrackingDataDirectory_proto_msgTypes,
	}.Build()
	File_TrackingDataDirectory_proto = out.File
	file_TrackingDataDirectory_proto_rawDesc = nil
	file_TrackingDataDirectory_proto_goTypes = nil
	file_TrackingDataDirectory_proto_depIdxs = nil
}
