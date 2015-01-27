<?php

// CRUD

//// Roles

function _rol_login_get_allowed_roles(){
	$role_names = user_roles(true);
	$used_roles = array();

	$list = db_select('rol_login_roles', 'rfr')
		->fields('rfr', array('rid'))
		->distinct()
		->execute()
		->fetchCol();

	foreach($list as $rid){
		if(isset($role_names[$rid])){
			$used_roles[$rid] = $role_names[$rid];
		}
	}

	return array_diff_assoc($role_names, $used_roles);
}

function _rol_login_list($reset = false){
	$list = &drupal_static(__FUNCTION__);
	$role_names = user_roles(true);

	if($reset || !$list){
		$list = array();

		$res = db_select('rol_login', 'rf')
			->fields('rf')
			->orderBy('weight', 'ASC')
			->execute()
			->fetchAll();

		foreach($res as $row){
			$relations = db_select('rol_login_roles', 'rfr')
				->fields('rfr', array('rid'))
				->condition('rfid', $row->rfid, '=')
				->execute()
				->fetchCol();

			$roles = array();
			foreach($relations as $rid){
				if(isset($role_names[$rid])){
					$roles[$rid] = $role_names[$rid];
				}
			}

			if(!empty($roles)){
				$list[$row->rfid] = $row;
				$list[$row->rfid]->roles = $roles;
			}
		}
	}

	return $list;
}

function _rol_login_save($rf){
	if(isset($rf->rfid)){
		$ret = _rol_login_update($rf);
	}else{
		$ret = _rol_login_add($rf);
	}

	return $ret;
}

function _rol_login_add($rf){
	$roles = user_roles(true);

	$list = _rol_login_list();
	$last = end($list);
	$weight = $last ? $last->weight + 1 : 0;

	$rfid = db_insert('rol_login')
		->fields(array(
			'login' => $rf->login,
			'registration' => $rf->registration,
			'profile' => $rf->profile,
			'weight' => $weight,
		))
		->execute();
	if($rfid){
		foreach($rf->roles as $rid => $name){
			db_insert('rol_login_roles')
				->fields(array(
					'rfid' => $rfid,
					'rid' => $rid
				))
				->execute();
		}
	}

	return true;
}

function _rol_login_update($rf){
	$fields = array();

	if(isset($rf->roles)){
		db_delete('rol_login_roles')
			->condition('rfid', $rf->rfid)
			->execute();
		foreach($rf->roles as $rid => $name){
			db_insert('rol_login_roles')
				->fields(array(
					'rfid' => $rf->rfid,
					'rid' => $rid
				))
				->execute();
		}
	}

	$fields['login'] = $rf->login;
	$fields['registration'] = $rf->registration;
	$fields['profile'] = $rf->profile;

	if(isset($rf->weight)){
		$fields['weight'] = $rf->weight;
	}

	if(!empty($fields)){
		db_update('rol_login')
			->fields($fields)
			->condition('rfid', $rf->rfid, '=')
			->execute();
	}

	return true;
}

function _rol_login_delete($rf){
	db_delete('rol_login')
		->condition('rfid', $rf->rfid)
		->execute();
	db_delete('rol_login_roles')
		->condition('rfid', $rf->rfid)
		->execute();

	return true;
}

function _rol_login_load($rfid){
	$role_names = user_roles(true);

	$res = db_select('rol_login', 'rf')
		->fields('rf')
		->condition('rfid', $rfid, '=')
		->orderBy('weight', 'asc')
		->execute()
		->fetchObject();

	$roles = array();
	if($res && $res->rfid){
		$relations = db_select('rol_login_roles', 'rfr')
			->fields('rfr', array('rid'))
			->condition('rfid', $res->rfid, '=')
			->execute()
			->fetchCol();

		foreach($relations as $rid){
			if(isset($role_names[$rid])){
				$roles[$rid] = $role_names[$rid];
			}
		}
	}else{
		$res = new stdClass;
		$res->rfid = null;
		$res->login = null;
		$res->registration = null;
		$res->profile = null;
		$res->weight = 0;
	}
	$res->roles = $roles;

	return $res;
}

//// Creation

function _rol_login_creation_list($reset = false){
	$list = &drupal_static(__FUNCTION__);
	$role_names = user_roles(true);

	if($reset || !$list){
		$list = array();

		$res = db_select('rol_login_creation', 'rc')
			->fields('rc')
			->orderBy('weight', 'ASC')
			->execute()
			->fetchAll();

		foreach($res as $row){
			$relations = db_select('rol_login_creation_roles', 'rcr')
				->fields('rcr', array('rid'))
				->condition('rcid', $row->rcid, '=')
				->execute()
				->fetchCol();

			$roles = array();
			foreach($relations as $rid){
				if(isset($role_names[$rid])){
					$roles[$rid] = $role_names[$rid];
				}
			}

			if(!empty($roles)){
				$list[$row->rcid] = $row;
				$list[$row->rcid]->roles = $roles;
			}
		}
	}

	return $list;
}

function _rol_login_creation_save($rc){
	if(isset($rc->rcid)){
		$ret = _rol_login_creation_update($rc);
	}else{
		$ret = _rol_login_creation_add($rc);
	}

	return $ret;
}

function _rol_login_creation_add($rc){
	$roles = user_roles(true);

	$list = _rol_login_creation_list();
	$last = end($list);
	$weight = $last ? $last->weight + 1 : 0;

	$rcid = db_insert('rol_login_creation')
		->fields(array(
			'url' => $rc->url,
			'weight' => $weight,
		))
		->execute();
	if($rcid){
		foreach($rc->roles as $rid => $name){
			db_insert('rol_login_creation_roles')
				->fields(array(
					'rcid' => $rcid,
					'rid' => $rid
				))
				->execute();
		}
	}

	return true;
}

function _rol_login_creation_update($rc){
	$fields = array();

	if(isset($rc->roles)){
		db_delete('rol_login_creation_roles')
			->condition('rcid', $rc->rcid)
			->execute();
		foreach($rc->roles as $rid => $name){
			db_insert('rol_login_creation_roles')
				->fields(array(
					'rcid' => $rc->rcid,
					'rid' => $rid
				))
				->execute();
		}
	}

	$fields['url'] = $rc->url;

	if(isset($rc->weight)){
		$fields['weight'] = $rc->weight;
	}

	if(!empty($fields)){
		db_update('rol_login_creation')
			->fields($fields)
			->condition('rcid', $rc->rcid, '=')
			->execute();
	}

	return true;
}

function _rol_login_creation_delete($rc){
	db_delete('rol_login_creation')
		->condition('rcid', $rc->rcid)
		->execute();
	db_delete('rol_login_creation_roles')
		->condition('rcid', $rc->rcid)
		->execute();

	return true;
}

function _rol_login_creation_load($rcid){
	$role_names = user_roles(true);

	$res = db_select('rol_login_creation', 'rc')
		->fields('rc')
		->condition('rcid', $rcid, '=')
		->execute()
		->fetchObject();

	$roles = array();
	if($res && $res->rcid){
		$relations = db_select('rol_login_creation_roles', 'rcr')
			->fields('rcr', array('rid'))
			->condition('rcid', $res->rcid, '=')
			->execute()
			->fetchCol();

		foreach($relations as $rid){
			if(isset($role_names[$rid])){
				$roles[$rid] = $role_names[$rid];
			}
		}
	}else{
		$res = new stdClass;
		$res->rcid = null;
		$res->url = null;
		$res->weight = 0;
	}
	$res->roles = $roles;

	return $res;
}

function _rol_creation_load($rcid){
	$role_names = user_roles(true);

	$res = db_select('rol_login_creation', 'rc')
		->fields('rc')
		->condition('rcid', $rcid, '=')
		->orderBy('weight', 'asc')
		->execute()
		->fetchObject();

	$roles = array();
	if($res && $res->rcid){
		$relations = db_select('rol_login_creation_roles', 'rcr')
			->fields('rcr', array('rid'))
			->condition('rcid', $res->rcid, '=')
			->execute()
			->fetchCol();

		foreach($relations as $rid){
			if(isset($role_names[$rid])){
				$roles[$rid] = $role_names[$rid];
			}
		}
	}else{
		$res = new stdClass;
		$res->rcid = null;
		$res->url = null;
		$res->weight = 0;
	}
	$res->roles = $roles;

	return $res;
}

// Utils

function _rol_login_get_user_settings($user = null){
	if(is_null($user)){
		global $user;
	}

	if($user && $user->uid){
		$list = _rol_login_list();
		foreach($list as $rf){
			foreach($user->roles as $rid => $role_name){
				if(isset($rf->roles[$rid]) || $user->uid == 1){
					return $rf;
				}
			}
		}
	}

	return false;
}