<?php

function rol_creation_settings_features_export($data, &$export, $module_name){
	$export['dependencies']['rol_login'] = 'rol_login';
	foreach ($data as $component) {
		$export['features']['rol_creation_settings'][$component] = $component;
	}
	return array();
}

function rol_creation_settings_features_export_options() {
	module_load_include('inc', 'rol_login', 'features/rol_creation_settings');

	$options = array();
	foreach(rol_creation_settings_load() as $component) {
		$options[$component->url] = $component->url;
	}

	return $options;
}

function rol_creation_settings_features_export_render($module_name, $data, $export = NULL) {
	module_load_include('inc', 'rol_login', 'features/rol_creation_settings');

	$code = array();
	$code[] = '  $rol_creation_settings = array();';
	$code[] = '';
	foreach($data as $url){
		$rc = rol_creation_settings_load($url);
		unset($rc->rcid);
		$code[] = '  $rol_creation_settings["'.$rc->url.'"] = '.features_var_export($rc, '  ').';';
	}
	$code[] = '';
	$code[] = '  return $rol_creation_settings;';
	$code = implode("\n", $code);
	return array('rol_creation_settings_defaults' => $code);
}

function rol_creation_settings_features_revert($module_name) {
	rol_creation_settings_features_rebuild($module_name);
}

function rol_creation_settings_features_rebuild($module_name) {
	module_load_include('inc', 'rol_login', 'features/rol_creation_settings');

	$rol_creation_settings = module_invoke($module_name, 'rol_creation_settings_defaults');
	if (!empty($rol_creation_settings)) {
		foreach ($rol_creation_settings as $settings) {
			rol_creation_settings_save($settings);
		}
	}
}
