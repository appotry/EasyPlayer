module.exports = function(grunt) {

  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
    includereplace : {
      main : {
        expand : true,
        cwd : "./",
        src : ["*.html","!common.html","!header.html","!footer.html"],
        dest : "./dist/"
      }
    },
    watch : {
      html : {
        files : ["./*.html"],
        tasks : ["includereplace"]
      }
    }
  });

  grunt.loadNpmTasks('grunt-include-replace');
  grunt.loadNpmTasks('grunt-contrib-watch');

  grunt.registerTask('default',['includereplace','watch']);

};