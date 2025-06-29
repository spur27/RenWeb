// add webview stuff
import * as project_package_json from '../package.json' with { type: "json" };
import { writeFileSync } from 'fs';
import Path from 'path';

let project_settings: {[key: string]: any} = project_package_json.default;
let default_project_settings: {[key: string]: any} = {
    "name": "Program Name",
    "version": "1.0.0",
    "description": "Description",
    "license": "License",
    "author": "Author",

    "main_page": "main",
    "src_dir_name": "src",
    "pages_dir_name": "pages",
}

console.log("Starting validator...");

for (const key in default_project_settings) {
    project_settings[key] = ((setting) => {
        if (setting == null || setting == undefined) return setting;
        console.log(`KEY FOUND for "${key}": ${setting}`);
        return setting;
    })(project_settings[key])         ??         ((default_setting) => {
        console.log(`NONEXISTENT KEY for "${key}"! Setting to default: ${default_setting}`);
        return default_setting;
    })(default_project_settings[key]);
}

writeFileSync("../package.json", JSON.stringify(project_settings, null, 2));
