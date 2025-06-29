// add webview stuff
import webpack, { Configuration } from 'webpack';
import HtmlWebpackPlugin from 'html-webpack-plugin';
import HtmlInlineScriptWebpackPlugin from 'html-inline-script-webpack-plugin';
import TerserWebpackPlugin from 'terser-webpack-plugin';
import { readdirSync, existsSync, readFileSync, PathLike, mkdirSync } from 'fs';
import Path from 'path';
import { LogLevel, Logger } from '../lib/logger/logger.js';
import Chalk from 'chalk';
import { homedir } from 'os';
import { execSync, spawn } from 'child_process';

const logger = new Logger("Builder", true, LogLevel.TRACE, Chalk.bold.magenta);

const warnAboutMissingKey = (key: string, default_v: {}) => {
    logger.warn(`"${key}" isn't in your ./package.json! Using the default value ("${default_v}")`);
    return default_v;
}
const throwCriticalError = (msg: any) => {
    logger.critical(msg);
    throw new Error(msg);
}
const runWebpack = async (config: Configuration, page_dir_name: string): Promise<void> => {
    return new Promise((resolve, reject) => {
        webpack(config, (err, stats) => {
            if (err) {
                reject(err);
                return;
            }
            const info = stats?.toJson();
            if (stats?.hasErrors()) {
                reject(info?.errors);
            }
            if (stats?.hasWarnings()) {
                logger.warn(info?.warnings);
            }
            resolve();
        });
    });
}


logger.debug("Starting builder...");
logger.trace(`Args are ${JSON.stringify(process.argv, null, 2)}`);
const project_root_dir = Path.join(import.meta.dirname, "../../../");
logger.trace(`Project root dir is '${project_root_dir}'`);

let project_settings: { [key: string]: any } = {};
try {
    const package_json_path = Path.join(project_root_dir, "package.json");
    project_settings = JSON.parse(readFileSync(package_json_path, 'utf8'));
    logger.info(`"./package.json" found at '${package_json_path}'`);
} catch (e) {
    throwCriticalError(e);
}

project_settings.src_dir_name = project_settings.src_dir_name ?? warnAboutMissingKey("src_dir_name", "src");
const src_path = Path.join(project_root_dir, `${project_settings.src_dir_name}`);
logger.trace(`Src path is '${src_path}'`);
if (!existsSync(src_path)) {
    throwCriticalError(`Source path at '${src_path}' doesn't exist! Can't build anything.`);
}

project_settings.pages_dir_name = project_settings.pages_dir_name ?? warnAboutMissingKey("pages_dir_name", "pages");
const pages_path = Path.join(src_path, `/${project_settings.pages_dir_name}`);
logger.trace(`Pages path is '${pages_path}'`);
if (!existsSync(pages_path)) {
    throwCriticalError(`Pages path at '${pages_path}' doesn't exist! Can't build anything.`);
}

const pages_dir_names = readdirSync(pages_path, 'utf8');
logger.trace(`Pages dirs are ${JSON.stringify(pages_dir_names, null, 2)}`);
project_settings.main_page = project_settings.main_page ?? warnAboutMissingKey("main_page", "main");
if (!pages_dir_names.includes(project_settings.main_page)) {
    throwCriticalError(`Main page "${project_settings.main_page}" is not a directory in the "${project_settings.pages_dir_name}" directory!`);
}

const entry_points = [
    "index.ts",
    "index.tsx",
    "index.js",
    "index.jsx",
    "script.ts",
    "script.js",
];
pages_dir_names.forEach(async (page_dir_name, index, arr) => {
    logger.info(`Beginning webpack for "${page_dir_name}"...`);
    let page_path = Path.join(pages_path, page_dir_name);
    let entry_point: (string | undefined) = undefined;
    readdirSync(page_path, 'utf8').forEach((el, index, arr) => {
        el = el.toLocaleLowerCase();
        if (entry_points.includes(el)) {
            if (entry_point != undefined) {
                logger.warn(`Another suitable entry point of name "${el}" was found. Ignoring...`);
                return;
            }
            entry_point = Path.join(page_path, el);
            logger.info(`Entry point found at '${entry_point}'`);
        }
    });
    if (entry_point == undefined) {
        logger.error(`No entry point found for page in '${page_path}' skipping...`);
    } else {
        logger.trace(`output: '${Path.join(project_root_dir, "build", "html", `${page_dir_name}.html`)}'`);
        const is_entry_point_html = (entry_point as string).toLowerCase().endsWith(".html");
        if (is_entry_point_html) logger.trace(`Proceeding with HTML plugin`);
        const config: Configuration = {
            entry: entry_point,
            output: {
                path: Path.join(project_root_dir, "build", "html"),
                filename: `${page_dir_name}.bundle.js`
            },
            mode: 'development',
            resolve: {
                extensions: ['.tsx', '.ts', '.jsx', '.js']
            },
            optimization: {
                runtimeChunk: false,
                splitChunks: false,
                minimize: true,
                minimizer: [
                    new TerserWebpackPlugin({
                        extractComments: false
                    })
                ]
            },
            module: {
                rules: [
                    {
                        test: /\.(ts|tsx|js|jsx)$/i,
                        exclude: /node_modules/,
                        use: {
                            loader: 'babel-loader',
                            options: {
                                presets: [
                                    '@babel/preset-env',
                                    '@babel/preset-react',
                                    '@babel/preset-typescript'
                                ]
                            }
                        }
                    },
                    {
                        test: /\.css$/i,
                        use: ['style-loader', 'css-loader']
                    },
                    {
                        test: /\.(png|jpe?g|gif|svg|woff2?|eot|ttf|otf)$/i,
                        type: 'asset/inline'
                    }
                ]
            },
            plugins: [
                new HtmlWebpackPlugin( 
                    (is_entry_point_html) ?
                        {
                            filename: `${page_dir_name}.html`,
                            template: entry_point,
                            inject: 'body',
                            scriptLoading: 'blocking',
                            minify: true
                        }
                    :
                        {
                            filename: `${page_dir_name}.html`,
                            templateContent: ({ htmlWebpackPlugin }) => `\
                                <!DOCTYPE html>
                                <html lang="en">
                                    <head>
                                    <meta charset="UTF-8">
                                    <meta name="viewport" content="width=device-width, initial-scale=1.0">
                                    </head>
                                    <body>
                                    <div id="root"></div>
                                    ${htmlWebpackPlugin.tags.bodyTags}
                                    </body>
                                </html>`,
                            inject: false,
                            scriptLoading: 'blocking',
                            minify: true
                        }
                ),
                new HtmlInlineScriptWebpackPlugin()
            ]
        };
        try {
            await runWebpack(config, page_dir_name);
        } catch (e) {
            logger.error(e);
        }
    }
});
logger.info("Finished with webpacking.");


project_settings.name = project_settings.name ?? warnAboutMissingKey("name", "Default Project Name");
const project_binary_name = (project_settings.name as string).toLowerCase().trim().replaceAll(/\s/g, "_");
logger.trace(`Project binary name is "${project_binary_name}"`);
const project_binary_path = Path.join(project_root_dir, "build", project_binary_name);
let emergency_stop = false;
do {
    if (!existsSync(project_binary_path)) {
        if (emergency_stop) {
            throwCriticalError(`Cannot find or generate engine binary. It should be at '${project_binary_path}'`);
        }
        logger.error(`No engine binary found at '${project_binary_path}'. Attempting to build one...`);
        const engine_path = Path.join(project_root_dir, "engine");
        const cmd = `cd ${engine_path} && scons`;
        logger.trace(`Running command '${cmd}'`);
        try {
            execSync(cmd);
        } catch (e) {
            logger.error(e);
        }
        emergency_stop = true;;
        continue;
    }
    break;
} while (true);

const project_process = spawn(project_binary_path);
// project_process.unref();
