<?php
function rol_login_settings_load($roles = array()){
	module_load_include('inc', 'rol_login');

	$settings = _rol_login_list(true);

	if($roles){
		sort($roles);
		$ret = false;
		foreach($settings as $rf){
			$rf_roles = array_values($rf->roles);
			sort($rf_roles);
			if($rf_roles == $roles){
				$ret = $rf;
				break;
			}
		}

		$settings = $ret;
	}

	return $settings;
}

function rol_login_settings_save($rol_login_settings){
	$rol_login_settings = (object) $rol_login_settings;

	$rol_login_settings_roles = array_values($rol_login_settings->roles);
	sort($rol_login_settings_roles);

	$settings = rol_login_settings_load();
	foreach($settings as $rfid => $rf){
		$rf_roles = array_values($rf->roles);
		sort($rf_roles);
		if($rf_roles == $rol_login_settings_roles){
			$rol_login_settings->rfid = $rf->rfid;
			break;
		}
	}

	module_load_include('inc', 'rol_login');
	_rol_login_save($rol_login_settings);
}
