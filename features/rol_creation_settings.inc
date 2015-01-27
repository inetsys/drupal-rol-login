<?php
function rol_creation_settings_load($url = false){
	module_load_include('inc', 'rol_login');

	if($url){
		$rcid = db_select('rol_login_creation', 'rc')
			->fields('rc', array('rcid'))
			->condition('url', $url, '=')
			->execute()
			->fetchField();
		$ret = _rol_login_creation_load($rcid);
	}else{
		$ret =  _rol_login_creation_list();
	}

	return $ret;
}

function rol_creation_settings_save($rol_creation_settings){
	$rol_creation_settings = (object) $rol_creation_settings;

	$settings = rol_creation_settings_load();
	foreach($settings as $rcid => $rc){
		if($rc->url == $rol_creation_settings->url){
			$rol_creation_settings->rcid = $rc->rcid;
		}
	}

	module_load_include('inc', 'rol_login');
	_rol_login_creation_save($rol_creation_settings);
}
