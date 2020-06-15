// Code generated by protoc-gen-go. DO NOT EDIT.
// versions:
// 	protoc-gen-go v1.24.0-devel
// 	protoc        v3.6.1
// source: Measurement.proto

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

type MeasurementType struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	ID   uint32 `protobuf:"varint,1,opt,name=ID,proto3" json:"ID,omitempty"`
	Name string `protobuf:"bytes,2,opt,name=name,proto3" json:"name,omitempty"`
}

func (x *MeasurementType) Reset() {
	*x = MeasurementType{}
	if protoimpl.UnsafeEnabled {
		mi := &file_Measurement_proto_msgTypes[0]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *MeasurementType) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*MeasurementType) ProtoMessage() {}

func (x *MeasurementType) ProtoReflect() protoreflect.Message {
	mi := &file_Measurement_proto_msgTypes[0]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use MeasurementType.ProtoReflect.Descriptor instead.
func (*MeasurementType) Descriptor() ([]byte, []int) {
	return file_Measurement_proto_rawDescGZIP(), []int{0}
}

func (x *MeasurementType) GetID() uint32 {
	if x != nil {
		return x.ID
	}
	return 0
}

func (x *MeasurementType) GetName() string {
	if x != nil {
		return x.Name
	}
	return ""
}

type Measurement struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	TagCloseUpURI string    `protobuf:"bytes,1,opt,name=tagCloseUpURI,proto3" json:"tagCloseUpURI,omitempty"`
	Type          uint32    `protobuf:"varint,2,opt,name=type,proto3" json:"type,omitempty"`
	Start         *Vector2D `protobuf:"bytes,3,opt,name=start,proto3" json:"start,omitempty"`
	End           *Vector2D `protobuf:"bytes,4,opt,name=end,proto3" json:"end,omitempty"`
	TagSizePx     float64   `protobuf:"fixed64,5,opt,name=tagSizePx,proto3" json:"tagSizePx,omitempty"`
}

func (x *Measurement) Reset() {
	*x = Measurement{}
	if protoimpl.UnsafeEnabled {
		mi := &file_Measurement_proto_msgTypes[1]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *Measurement) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*Measurement) ProtoMessage() {}

func (x *Measurement) ProtoReflect() protoreflect.Message {
	mi := &file_Measurement_proto_msgTypes[1]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use Measurement.ProtoReflect.Descriptor instead.
func (*Measurement) Descriptor() ([]byte, []int) {
	return file_Measurement_proto_rawDescGZIP(), []int{1}
}

func (x *Measurement) GetTagCloseUpURI() string {
	if x != nil {
		return x.TagCloseUpURI
	}
	return ""
}

func (x *Measurement) GetType() uint32 {
	if x != nil {
		return x.Type
	}
	return 0
}

func (x *Measurement) GetStart() *Vector2D {
	if x != nil {
		return x.Start
	}
	return nil
}

func (x *Measurement) GetEnd() *Vector2D {
	if x != nil {
		return x.End
	}
	return nil
}

func (x *Measurement) GetTagSizePx() float64 {
	if x != nil {
		return x.TagSizePx
	}
	return 0
}

var File_Measurement_proto protoreflect.FileDescriptor

var file_Measurement_proto_rawDesc = []byte{
	0x0a, 0x11, 0x4d, 0x65, 0x61, 0x73, 0x75, 0x72, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x2e, 0x70, 0x72,
	0x6f, 0x74, 0x6f, 0x12, 0x10, 0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64,
	0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x1a, 0x0e, 0x56, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x32, 0x64, 0x2e,
	0x70, 0x72, 0x6f, 0x74, 0x6f, 0x22, 0x35, 0x0a, 0x0f, 0x4d, 0x65, 0x61, 0x73, 0x75, 0x72, 0x65,
	0x6d, 0x65, 0x6e, 0x74, 0x54, 0x79, 0x70, 0x65, 0x12, 0x0e, 0x0a, 0x02, 0x49, 0x44, 0x18, 0x01,
	0x20, 0x01, 0x28, 0x0d, 0x52, 0x02, 0x49, 0x44, 0x12, 0x12, 0x0a, 0x04, 0x6e, 0x61, 0x6d, 0x65,
	0x18, 0x02, 0x20, 0x01, 0x28, 0x09, 0x52, 0x04, 0x6e, 0x61, 0x6d, 0x65, 0x22, 0xc5, 0x01, 0x0a,
	0x0b, 0x4d, 0x65, 0x61, 0x73, 0x75, 0x72, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x12, 0x24, 0x0a, 0x0d,
	0x74, 0x61, 0x67, 0x43, 0x6c, 0x6f, 0x73, 0x65, 0x55, 0x70, 0x55, 0x52, 0x49, 0x18, 0x01, 0x20,
	0x01, 0x28, 0x09, 0x52, 0x0d, 0x74, 0x61, 0x67, 0x43, 0x6c, 0x6f, 0x73, 0x65, 0x55, 0x70, 0x55,
	0x52, 0x49, 0x12, 0x12, 0x0a, 0x04, 0x74, 0x79, 0x70, 0x65, 0x18, 0x02, 0x20, 0x01, 0x28, 0x0d,
	0x52, 0x04, 0x74, 0x79, 0x70, 0x65, 0x12, 0x30, 0x0a, 0x05, 0x73, 0x74, 0x61, 0x72, 0x74, 0x18,
	0x03, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x1a, 0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72,
	0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x2e, 0x56, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x32,
	0x64, 0x52, 0x05, 0x73, 0x74, 0x61, 0x72, 0x74, 0x12, 0x2c, 0x0a, 0x03, 0x65, 0x6e, 0x64, 0x18,
	0x04, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x1a, 0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72,
	0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x2e, 0x56, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x32,
	0x64, 0x52, 0x03, 0x65, 0x6e, 0x64, 0x12, 0x1c, 0x0a, 0x09, 0x74, 0x61, 0x67, 0x53, 0x69, 0x7a,
	0x65, 0x50, 0x78, 0x18, 0x05, 0x20, 0x01, 0x28, 0x01, 0x52, 0x09, 0x74, 0x61, 0x67, 0x53, 0x69,
	0x7a, 0x65, 0x50, 0x78, 0x62, 0x06, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x33,
}

var (
	file_Measurement_proto_rawDescOnce sync.Once
	file_Measurement_proto_rawDescData = file_Measurement_proto_rawDesc
)

func file_Measurement_proto_rawDescGZIP() []byte {
	file_Measurement_proto_rawDescOnce.Do(func() {
		file_Measurement_proto_rawDescData = protoimpl.X.CompressGZIP(file_Measurement_proto_rawDescData)
	})
	return file_Measurement_proto_rawDescData
}

var file_Measurement_proto_msgTypes = make([]protoimpl.MessageInfo, 2)
var file_Measurement_proto_goTypes = []interface{}{
	(*MeasurementType)(nil), // 0: fort.myrmidon.pb.MeasurementType
	(*Measurement)(nil),     // 1: fort.myrmidon.pb.Measurement
	(*Vector2D)(nil),        // 2: fort.myrmidon.pb.Vector2d
}
var file_Measurement_proto_depIdxs = []int32{
	2, // 0: fort.myrmidon.pb.Measurement.start:type_name -> fort.myrmidon.pb.Vector2d
	2, // 1: fort.myrmidon.pb.Measurement.end:type_name -> fort.myrmidon.pb.Vector2d
	2, // [2:2] is the sub-list for method output_type
	2, // [2:2] is the sub-list for method input_type
	2, // [2:2] is the sub-list for extension type_name
	2, // [2:2] is the sub-list for extension extendee
	0, // [0:2] is the sub-list for field type_name
}

func init() { file_Measurement_proto_init() }
func file_Measurement_proto_init() {
	if File_Measurement_proto != nil {
		return
	}
	file_Vector2d_proto_init()
	if !protoimpl.UnsafeEnabled {
		file_Measurement_proto_msgTypes[0].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*MeasurementType); i {
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
		file_Measurement_proto_msgTypes[1].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*Measurement); i {
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
			RawDescriptor: file_Measurement_proto_rawDesc,
			NumEnums:      0,
			NumMessages:   2,
			NumExtensions: 0,
			NumServices:   0,
		},
		GoTypes:           file_Measurement_proto_goTypes,
		DependencyIndexes: file_Measurement_proto_depIdxs,
		MessageInfos:      file_Measurement_proto_msgTypes,
	}.Build()
	File_Measurement_proto = out.File
	file_Measurement_proto_rawDesc = nil
	file_Measurement_proto_goTypes = nil
	file_Measurement_proto_depIdxs = nil
}
