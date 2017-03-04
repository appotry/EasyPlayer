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

  // grunt.loadNpmTasks('grunt-contrib-copy');
  grunt.loadNpmTasks('grunt-include-replace');
  grunt.loadNpmTasks('grunt-contrib-watch');
  // grunt.loadNpmTasks('grunt-contrib-clean');

  grunt.registerTask('default',['includereplace','watch']);
  // grunt.registerTask('release', ['clean','copy','default']);

};