/*
Navicat MySQL Data Transfer

Source Server         : LocalMySql
Source Server Version : 50171
Source Host           : 192.168.1.76:3306
Source Database       : test

Target Server Type    : MYSQL
Target Server Version : 50171
File Encoding         : 65001

Date: 2016-03-18 16:10:37
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for IllegalInfo
-- ----------------------------
DROP TABLE IF EXISTS `IllegalInfo`;
CREATE TABLE `IllegalInfo` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `plate` varchar(20) CHARACTER SET gbk DEFAULT NULL COMMENT '车牌',
  `color` varchar(20) CHARACTER SET gbk DEFAULT NULL COMMENT '车身颜色',
  `type` tinyint(1) DEFAULT '1' COMMENT '监测类型，1违停，2逆行。默认1',
  `preset` tinyint(2) DEFAULT '1' COMMENT '球机可转动，可设置多个预置点，默认位置1',
  `picname` varchar(255) CHARACTER SET gbk DEFAULT NULL COMMENT '违章发生抓拍的图片名称',
  `picname1` varchar(255) CHARACTER SET gbk DEFAULT NULL COMMENT '球机第一次拉近执拍摄',
  `picname2` varchar(255) CHARACTER SET gbk DEFAULT NULL,
  `picname3` varchar(255) CHARACTER SET gbk DEFAULT NULL,
  `picname4` varchar(255) CHARACTER SET gbk DEFAULT NULL,
  `time` datetime DEFAULT NULL COMMENT '规章发生时间',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=59 DEFAULT CHARSET=latin1;
