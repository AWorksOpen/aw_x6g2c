﻿
/*This file is generated by code generator*/

#include "controller.h"
#include "mvvm/base/view_model.h"

#ifndef TK_CONTROLLER_VIEW_MODEL_H
#define TK_CONTROLLER_VIEW_MODEL_H

BEGIN_C_DECLS
/**
 * @class controller_view_model_t
 *
 * view model of controller
 *
 */
typedef struct _controller_view_model_t {
  view_model_t view_model;

  /*model object*/
  controller_t* acontroller;
} controller_view_model_t;

/**
 * @method controller_view_model_create
 * 创建controller view model对象。
 *
 * @annotation ["constructor"]
 * @param {navigator_request_t*} req 请求参数。
 *
 * @return {view_model_t} 返回view_model_t对象。
 */
view_model_t* controller_view_model_create(navigator_request_t* req);

/**
 * @method controller_view_model_create_with
 * 创建controller view model对象。
 *
 * @annotation ["constructor"]
 * @param {controller_t*}  acontroller controller对象。
 *
 * @return {view_model_t} 返回view_model_t对象。
 */
view_model_t* controller_view_model_create_with(controller_t* acontroller);

/**
 * @method controller_view_model_attach
 * 关联到controller对象。
 *
 * @param {view_model_t*} view_model view_model对象。
 * @param {controller_t*} controller controller对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t controller_view_model_attach(view_model_t* vm, controller_t* acontroller);

END_C_DECLS

#endif /*TK_CONTROLLER_VIEW_MODEL_H*/
