<?php

function rol_login_settings_features_export($data, &$export, $module_name){
	$export['dependencies']['rol_login'] = 'rol_login';
	foreach ($data as $component) {
		$export['features']['rol_login_settings'][$component] = $component;
	}
	return array();
}

function rol_login_settings_features_export_options() {
	module_load_include('inc', 'rol_login', 'features/rol_login_settings');

	$options = array();
	foreach(rol_login_settings_load() as $component){
		$options[implode('-', $component->roles)] = implode(', ', $component->roles);
	}

	return $options;
}

function rol_login_settings_features_export_render($module_name, $data, $export = NULL) {
	module_load_include('inc', 'rol_login', 'features/rol_login_settings');

	$code = array();
	$code[] = '  $rol_login_settings = array();';
	$code[] = '';
	foreach($data as $key){
		$roles = explode('-', $key);
		$rf = rol_login_settings_load($roles);
		unset($rf->rfid);
		$code[] = '  $rol_login_settings["'.$key.'"] = '.features_var_export($rf, '  ').';';
	}
	$code[] = '';
	$code[] = '  return $rol_login_settings;';
	$code = implode("\n", $code);
	return array('rol_login_settings_defaults' => $code);
}

function rol_login_settings_features_revert($module_name) {
	rol_login_settings_features_rebuild($module_name);
}

function rol_login_settings_features_rebuild($module_name) {
	module_load_include('inc', 'rol_login', 'features/rol_login_settings');

	$rol_login_settings = module_invoke($module_name, 'rol_login_settings_defaults');
	if (!empty($rol_login_settings)) {
		foreach ($rol_login_settings as $settings) {
			rol_login_settings_save($settings);
		}
	}
}
