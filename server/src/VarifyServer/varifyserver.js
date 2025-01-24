const grpc = require('@grpc/grpc-js')
const message_proto = require('./proto')
const const_module = require('./const')
const emailModule = require('./email')
const { v4: uuidv4 } = require('uuid');
//const redis_module = require('./redis')


async function GetVarifyCode(call, callback) {   //对应proto文件中的GetVarifyCode方法
    console.log("email is ", call.request.email)
    try{
        uniqueId = uuidv4();

        // let query_res = await redis_module.GetRedis(const_module.code_prefix+call.request.email);
        // console.log("query_res is ", query_res)
        // let uniqueId = query_res;
        // if(query_res == null){
        //     uniqueId = uuidv4();
        //     if (uniqueId.length > 4) {
        //         uniqueId = uniqueId.substring(0, 4);
        //     } 
        //     let bres = await redis_module.SetRedisExpire(const_module.code_prefix+call.request.email, uniqueId,600)
        //     if(!bres){
        //         callback(null, { email:  call.request.email,  //是grpc的返回
        //             error:const_module.Errors.RedisErr
        //         });
        //         return;
        //     }
        // }
        
        console.log("uniqueId is ", uniqueId)
        let text_str =  '您的验证码为'+ uniqueId +'请三分钟内完成注册'
        //发送邮件
        let mailOptions = {
            from: '2824614566@qq.com',
            to: call.request.email,
            subject: '验证码',
            text: text_str,
        };

        let send_res = await emailModule.SendMail(mailOptions);  //await 等待异步接口返回直到 Promise 完成
        console.log("send res is ", send_res)

        // if(send_res == null){
        //     callback(null, { email:  call.request.email,
        //         error:const_module.Errors.Exception
        //     });
        //     return;
        // }
        callback(null, { email:  call.request.email,
            error:const_module.Errors.Success
        });

    }catch(error){   //捕获异常(reject的返回)
        console.log("catch error is ", error)

        callback(null, { email:  call.request.email,
            error:const_module.Errors.Exception
        }); 
    }

}

function main() {
    var server = new grpc.Server()
    server.addService(message_proto.VarifyService.service, { GetVarifyCode: GetVarifyCode }) //添加服务  GetVarifyCode: GetVarifyCode 服务名:方法名
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), () => {  //绑定地址和端口
        //server.start()   新版本可以省略
        console.log('varify_grpc_server started')        
    })
}

main()