/* nodejs可以自动解析proto文件，安装@grpc/proto-loader库
   nodemailer库发送邮件
   grpc-js库实现grpc通信
*/

const path = require('path')         //nodejs里面用require来包含一个库，类似C++的include
const grpc = require('@grpc/grpc-js')
const protoLoader = require('@grpc/proto-loader')

const PROTO_PATH = path.join(__dirname , '../common/message.proto')  //__dirname是当前文件所在的目录
//protoLoader.loadSync() 同步加载proto文件，返回一个packageDefinition对象         
const packageDefinition = protoLoader.loadSync(PROTO_PATH, { 
    //keepcase:true 保持字段名大小写 longs:String 长整型转换为字符串 enums:String 枚举类型转换为字符串 defaults:true 默认值 true oneofs:true 一个of字段 true
    keepCase: true, longs: String, enums: String, defaults: true, oneofs: true }) 
//grpc.loadPackageDefinition() 将packageDefinition对象转换为protoDescriptor对象
const protoDescriptor = grpc.loadPackageDefinition(packageDefinition)

//protoDescriptor.message 是proto文件中定义的message对象
const message_proto = protoDescriptor.message

//导出message_proto对象  类似于C++的extern
module.exports = message_proto
