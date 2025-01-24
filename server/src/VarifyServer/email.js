const nodemailer = require('nodemailer');
const config_module = require("./config")

/**
 * 创建发送邮件的代理
 */
let transport = nodemailer.createTransport({
    host: 'smtp.qq.com',
    port: 465,
    secure: true,
    auth: {
        user: config_module.email_user, // 发送方邮箱地址
        pass: config_module.email_pass // 邮箱授权码或者密码
    }
});

/**
 * 发送邮件的函数
 * @param {*} mailOptions_ 发送邮件的参数
 * @returns 
 * 
 * 这里的promis类似于c++的future，我们就可以在需要的时候获取值（也就是同步运行）
 * 在c++中就用的.get()或者wait()，同时要是std::launch::deferred，那在js中是用await
 */
function SendMail(mailOptions_){
    
    return new Promise(function(resolve, reject){   //promise包装一下

        transport.sendMail(mailOptions_, function(error, info){
            if (error) {
                console.log(error);
                reject(error);
            } else {
                console.log('邮件已成功发送：' + info.response);
                resolve(info.response)
            }
        });

    })

}

module.exports.SendMail = SendMail