<?php

function _account_fields_filter_fields(&$form, $roles){
	$config = variable_get('account_fields', array());
	if(empty($config)){
		return;
	}

	foreach($form as $key => $data){
		if($instance = field_info_instance('user', $key, 'user')){
			$fid = $instance['field_id'];
			$name = $instance['field_name'];
			$visible = false;
			$required = false;

			foreach($roles as $rid => $rol){
				$field_config = isset($config[$rid]) && isset($config[$rid][$name]) ? $config[$rid][$name] : false;
				if($field_config){
					$visible = $visible || $field_config['visible'];
					$required = $required || ($visible && $field_config['required']);
				}else{
					$visible = $visible || !$instance['deleted'];
					$required = $required || $visible && $instance['required'];
				}
			}

			if(!$visible){
				$form[$key]['#access'] = false;
			}elseif($required){
				if(isset($form[$key][LANGUAGE_NONE][0]['value'])){
					$form[$key][LANGUAGE_NONE][0]['value']['#required'] = true;
				}
				if(isset($form[$key][LANGUAGE_NONE][0]['email'])){
					$form[$key][LANGUAGE_NONE][0]['email']['#required'] = true;
				}
			}
		}
	}
}

function _account_fields_field_access($field, $roles){
	global $user;

	// El usuario 1 puede ver todos los campos de cualquier otro usuario que no sea él mismo
	if($user && $user->uid == 1 && arg() != explode('/', variable_get('rol_login_profile_path', 'user/1'))){
		return true;
	}

	$visible = false;
	$config = variable_get('account_fields', array());
	if(!empty($config)){
		$fid = $field['field_id'];
		$name = $field['field_name'];

		foreach($roles as $rid => $rol){
			$field_config = isset($config[$rid]) && isset($config[$rid][$name]) ? $config[$rid][$name] : false;
			if($field_config){
				$visible = $visible || $field_config['visible'];
			}else{
				$visible = $visible || !$field['deleted'];
			}
		}
	}

	return $visible;
}

function _account_fields_field_required($field, $roles){
	global $user;

	// Para el usuario 1 ningún campo es obligatorio
	if($user && $user->uid == 1 && arg() != explode('/', variable_get('rol_login_profile_path', 'user/1'))){
		return false;
	}

	$required = false;
	$config = variable_get('account_fields', array());

	if(!empty($config)){
		$fid = $field['field_id'];
		$name = $field['field_name'];

		foreach($roles as $rid => $rol){
			$field_config = isset($config[$rid]) && isset($config[$rid][$name]) ? $config[$rid][$name] : false;
			if($field_config){
				$required = $field_config['required'];
			}else{
				$required = $field['required'];
			}

			if($required){
				break;
			}
		}
	}

	return $required;
}

function _check_user_required_fields($user){
	$roles = $user->roles;
	$account = user_load($user->uid);

	$instances = _field_invoke_get_instances('user', 'user', array('default' => TRUE, 'deleted' => FALSE));
	foreach($instances as $field_name => $instance){
		if(_account_fields_field_required($instance, $roles) && empty($account->$field_name)){
			return false;
		}
	}

	return true;
}