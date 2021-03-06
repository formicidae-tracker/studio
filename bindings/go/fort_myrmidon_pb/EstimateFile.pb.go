// Code generated by protoc-gen-go. DO NOT EDIT.
// versions:
// 	protoc-gen-go v1.24.0-devel
// 	protoc        v3.6.1
// source: EstimateFile.proto

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

type EstimateHeader struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	MajorVersion uint32 `protobuf:"varint,1,opt,name=MajorVersion,proto3" json:"MajorVersion,omitempty"`
	MinorVersion uint32 `protobuf:"varint,2,opt,name=MinorVersion,proto3" json:"MinorVersion,omitempty"`
}

func (x *EstimateHeader) Reset() {
	*x = EstimateHeader{}
	if protoimpl.UnsafeEnabled {
		mi := &file_EstimateFile_proto_msgTypes[0]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *EstimateHeader) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*EstimateHeader) ProtoMessage() {}

func (x *EstimateHeader) ProtoReflect() protoreflect.Message {
	mi := &file_EstimateFile_proto_msgTypes[0]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use EstimateHeader.ProtoReflect.Descriptor instead.
func (*EstimateHeader) Descriptor() ([]byte, []int) {
	return file_EstimateFile_proto_rawDescGZIP(), []int{0}
}

func (x *EstimateHeader) GetMajorVersion() uint32 {
	if x != nil {
		return x.MajorVersion
	}
	return 0
}

func (x *EstimateHeader) GetMinorVersion() uint32 {
	if x != nil {
		return x.MinorVersion
	}
	return 0
}

type Estimate struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Frame           uint64    `protobuf:"varint,1,opt,name=Frame,proto3" json:"Frame,omitempty"`
	Tag             uint32    `protobuf:"varint,2,opt,name=Tag,proto3" json:"Tag,omitempty"`
	TagPhysicalSize float64   `protobuf:"fixed64,3,opt,name=TagPhysicalSize,proto3" json:"TagPhysicalSize,omitempty"`
	Start           *Vector2D `protobuf:"bytes,4,opt,name=Start,proto3" json:"Start,omitempty"`
	End             *Vector2D `protobuf:"bytes,5,opt,name=End,proto3" json:"End,omitempty"`
}

func (x *Estimate) Reset() {
	*x = Estimate{}
	if protoimpl.UnsafeEnabled {
		mi := &file_EstimateFile_proto_msgTypes[1]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *Estimate) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*Estimate) ProtoMessage() {}

func (x *Estimate) ProtoReflect() protoreflect.Message {
	mi := &file_EstimateFile_proto_msgTypes[1]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use Estimate.ProtoReflect.Descriptor instead.
func (*Estimate) Descriptor() ([]byte, []int) {
	return file_EstimateFile_proto_rawDescGZIP(), []int{1}
}

func (x *Estimate) GetFrame() uint64 {
	if x != nil {
		return x.Frame
	}
	return 0
}

func (x *Estimate) GetTag() uint32 {
	if x != nil {
		return x.Tag
	}
	return 0
}

func (x *Estimate) GetTagPhysicalSize() float64 {
	if x != nil {
		return x.TagPhysicalSize
	}
	return 0
}

func (x *Estimate) GetStart() *Vector2D {
	if x != nil {
		return x.Start
	}
	return nil
}

func (x *Estimate) GetEnd() *Vector2D {
	if x != nil {
		return x.End
	}
	return nil
}

var File_EstimateFile_proto protoreflect.FileDescriptor

var file_EstimateFile_proto_rawDesc = []byte{
	0x0a, 0x12, 0x45, 0x73, 0x74, 0x69, 0x6d, 0x61, 0x74, 0x65, 0x46, 0x69, 0x6c, 0x65, 0x2e, 0x70,
	0x72, 0x6f, 0x74, 0x6f, 0x12, 0x10, 0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d, 0x69,
	0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x1a, 0x0e, 0x56, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x32, 0x64,
	0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x22, 0x58, 0x0a, 0x0e, 0x45, 0x73, 0x74, 0x69, 0x6d, 0x61,
	0x74, 0x65, 0x48, 0x65, 0x61, 0x64, 0x65, 0x72, 0x12, 0x22, 0x0a, 0x0c, 0x4d, 0x61, 0x6a, 0x6f,
	0x72, 0x56, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x18, 0x01, 0x20, 0x01, 0x28, 0x0d, 0x52, 0x0c,
	0x4d, 0x61, 0x6a, 0x6f, 0x72, 0x56, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x12, 0x22, 0x0a, 0x0c,
	0x4d, 0x69, 0x6e, 0x6f, 0x72, 0x56, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x18, 0x02, 0x20, 0x01,
	0x28, 0x0d, 0x52, 0x0c, 0x4d, 0x69, 0x6e, 0x6f, 0x72, 0x56, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e,
	0x22, 0xbc, 0x01, 0x0a, 0x08, 0x45, 0x73, 0x74, 0x69, 0x6d, 0x61, 0x74, 0x65, 0x12, 0x14, 0x0a,
	0x05, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x18, 0x01, 0x20, 0x01, 0x28, 0x04, 0x52, 0x05, 0x46, 0x72,
	0x61, 0x6d, 0x65, 0x12, 0x10, 0x0a, 0x03, 0x54, 0x61, 0x67, 0x18, 0x02, 0x20, 0x01, 0x28, 0x0d,
	0x52, 0x03, 0x54, 0x61, 0x67, 0x12, 0x28, 0x0a, 0x0f, 0x54, 0x61, 0x67, 0x50, 0x68, 0x79, 0x73,
	0x69, 0x63, 0x61, 0x6c, 0x53, 0x69, 0x7a, 0x65, 0x18, 0x03, 0x20, 0x01, 0x28, 0x01, 0x52, 0x0f,
	0x54, 0x61, 0x67, 0x50, 0x68, 0x79, 0x73, 0x69, 0x63, 0x61, 0x6c, 0x53, 0x69, 0x7a, 0x65, 0x12,
	0x30, 0x0a, 0x05, 0x53, 0x74, 0x61, 0x72, 0x74, 0x18, 0x04, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x1a,
	0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70,
	0x62, 0x2e, 0x56, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x32, 0x64, 0x52, 0x05, 0x53, 0x74, 0x61, 0x72,
	0x74, 0x12, 0x2c, 0x0a, 0x03, 0x45, 0x6e, 0x64, 0x18, 0x05, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x1a,
	0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70,
	0x62, 0x2e, 0x56, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x32, 0x64, 0x52, 0x03, 0x45, 0x6e, 0x64, 0x62,
	0x06, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x33,
}

var (
	file_EstimateFile_proto_rawDescOnce sync.Once
	file_EstimateFile_proto_rawDescData = file_EstimateFile_proto_rawDesc
)

func file_EstimateFile_proto_rawDescGZIP() []byte {
	file_EstimateFile_proto_rawDescOnce.Do(func() {
		file_EstimateFile_proto_rawDescData = protoimpl.X.CompressGZIP(file_EstimateFile_proto_rawDescData)
	})
	return file_EstimateFile_proto_rawDescData
}

var file_EstimateFile_proto_msgTypes = make([]protoimpl.MessageInfo, 2)
var file_EstimateFile_proto_goTypes = []interface{}{
	(*EstimateHeader)(nil), // 0: fort.myrmidon.pb.EstimateHeader
	(*Estimate)(nil),       // 1: fort.myrmidon.pb.Estimate
	(*Vector2D)(nil),       // 2: fort.myrmidon.pb.Vector2d
}
var file_EstimateFile_proto_depIdxs = []int32{
	2, // 0: fort.myrmidon.pb.Estimate.Start:type_name -> fort.myrmidon.pb.Vector2d
	2, // 1: fort.myrmidon.pb.Estimate.End:type_name -> fort.myrmidon.pb.Vector2d
	2, // [2:2] is the sub-list for method output_type
	2, // [2:2] is the sub-list for method input_type
	2, // [2:2] is the sub-list for extension type_name
	2, // [2:2] is the sub-list for extension extendee
	0, // [0:2] is the sub-list for field type_name
}

func init() { file_EstimateFile_proto_init() }
func file_EstimateFile_proto_init() {
	if File_EstimateFile_proto != nil {
		return
	}
	file_Vector2d_proto_init()
	if !protoimpl.UnsafeEnabled {
		file_EstimateFile_proto_msgTypes[0].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*EstimateHeader); i {
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
		file_EstimateFile_proto_msgTypes[1].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*Estimate); i {
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
			RawDescriptor: file_EstimateFile_proto_rawDesc,
			NumEnums:      0,
			NumMessages:   2,
			NumExtensions: 0,
			NumServices:   0,
		},
		GoTypes:           file_EstimateFile_proto_goTypes,
		DependencyIndexes: file_EstimateFile_proto_depIdxs,
		MessageInfos:      file_EstimateFile_proto_msgTypes,
	}.Build()
	File_EstimateFile_proto = out.File
	file_EstimateFile_proto_rawDesc = nil
	file_EstimateFile_proto_goTypes = nil
	file_EstimateFile_proto_depIdxs = nil
}
