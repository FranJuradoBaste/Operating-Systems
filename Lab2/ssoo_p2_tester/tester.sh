CalculateSimilarity() {
    bash_file=$1
    scripter_file=$2

    files_exist=1
    if [ ! -f ${bash_file} ]; then
        echo "${bash_file} does not exist."
        files_exist=0
    fi

    if [ ! -f ${scripter_file} ]; then
        echo "${scripter_file} does not exist."
        files_exist=0
    fi

    similarity=0
    if [ $files_exist == "1" ]; then
        # Count the total number of lines in both files
        total_lines=$(cat $bash_file $scripter_file | wc -l)
        # Calculate similarity percentage
        differing_lines=$(diff -u $bash_file $scripter_file | tail -n $total_lines | grep -E "^\+|^\-" | wc -l)
        # echo "$scripter_file - $bash_file, total lines=$total_lines, differing_lines=$differing_lines"
        if [ $total_lines = "0" ]; then
            echo "Error: division by 0 is not allowed"
        else
            similarity=$((100 - (100 * differing_lines) / total_lines))
        fi
    fi

    return $similarity
}

# Create test environment
dir_name_env_stu="test_student"
dir_name_env_shell="test_shell"
output_shell="salida_shell.txt"
output_stu="salida_stu.txt"
error_shell="error_shell.txt"
error_stu="error_stu.txt"

# Student code
# Check the number of arguments
if [ $# != 2 ]; then
    echo "[ENG] Usage: $0 ssoo_p2_NIA1_NIA2_NI3.zip <test_level>, see 'README-ENG.md' for more details."
    echo "[ESP] Uso: $0 ssoo_p2_NIA1_NIA2_NI3.zip <nivel_de_prueba>,  véase 'README-ESP.md' para más detalles."
    exit 1
fi

zip_filename="$1"
test_level=$2

# Check if the zip exists.
if [ ! -f $zip_filename ]; then
    echo "[ENG] $zip_filename does not exist."
    echo "[ESP] $zip_filename no existe."
    exit 1
fi

# Check the format of the zip file.
pattern="^ssoo_p2_([0-9]+)(_[0-9]+)?(_[0-9]+)?\.zip$"
if echo "$zip_filename" | grep -qE "$pattern"; then
    #echo "Valid format: $zip_filename"
    NIA1=$(echo "$zip_filename" | sed -E 's/^ssoo_p2_([0-9]+)(_[0-9]+)?(_[0-9]+)?\.zip$/\1/')
    NIA2=$(echo "$zip_filename" | sed -E 's/^ssoo_p2_([0-9]+)(_[0-9]+)?(_[0-9]+)?\.zip$/\2/' | sed 's/^_//')
    NIA3=$(echo "$zip_filename" | sed -E 's/^ssoo_p2_([0-9]+)(_[0-9]+)?(_[0-9]+)?\.zip$/\3/' | sed 's/^_//')

    echo "NIA1=$NIA1, NIA2=${NIA2:-None}, NIA3=${NIA3:-None}"
else
    echo "[ENG] Invalid format: $zip_filename, the expected format is ssoo_p2_NIA1_NIA2_NI3.zip"
    echo "[ESP] Formato no válido: $zip_filename, el formato esperado es ssoo_p2_NIA1_NIA2_NI3.zip"
    exit 1
fi

# Unzip the student code
out_dir=$(basename $zip_filename .zip)

if [ -d $out_dir ]; then
    rm -rf $out_dir
fi

unzip -o $1 -d $out_dir

# Move to the directory where the student code is
cd $out_dir

# Compile the student code
make

# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "[ENG] Compilation failed."
    echo "[ESP] La compilación ha fallado."
    exit 1
fi

echo "Reading autores.txt"
authors_file=autores.txt
if [ ! -f ${authors_file} ]; then
    echo "Missing $authors_file file"
    echo "Falta el fichero $authors_file"
    exit 1
else
    echo "[ENG] NIA,Surname,First name"
    echo "[ESP] NIA,Apellidos,Nombre"
    cat $authors_file
fi

# Read the file autores.txt line by line
pattern="^[0-9]+,[A-Za-z]+,[A-Za-z]+$"
line_number=0
all_valid=1
while IFS= read -r line; do
    line_number=$((line_number + 1))
    if [ -z $line ]; then
        echo "[ENG] Skipping empty line $line_number."
        echo "[ESP] Omitiendo la línea vacía $line_number."
        continue
    fi
    if ! echo "$line" | grep -qE "$pattern"; then
        echo "[ENG] Error: Line $line_number has an invalid format: $line"
        echo "[ESP] Error: La línea $line_number tiene un formato inválido: $line"
        all_valid=0
    fi
done <"$authors_file"

# Check if all lines are valid
if [ $all_valid = "0" ]; then
    exit 1
fi

# Copy test scripts to the student directory
cp -r ../InputScripts/ .

# Remove previous outputs if they exist
if [ -f $output_shell ]; then
    rm $output_shell
fi

if [ -f $output_stu ]; then
    rm $output_stu
fi

if [ -f $error_shell ]; then
    rm $error_shell
fi

if [ -f $error_stu ]; then
    rm $error_stu
fi

if [ ${test_level} = 1 ]; then
    echo "[ENG] Test level 1"
    echo "[ESP] Nivel de prueba 1"
    in_script_file_name_stu=1_basic_scripter.sh
    in_script_file_name_bash=1_basic_bash.sh
elif [ ${test_level} = 2 ]; then
    echo "[ENG] Test level 2"
    echo "[ESP] Nivel de prueba 2"
    in_script_file_name_stu=2_medium_scripter.sh
    in_script_file_name_bash=2_medium_bash.sh
else
    echo "[ENG] Test level not allowed, valid inputs are 1 or 2. See 'README-ENG.md' for more details."
    echo "[ESP] Nivel de prueba no permitido, las entradas válidas son 1 ó 2. Véase 'README-ESP.md' para más detalles."
    exit 0
fi

for i in $dir_name_env_shell $dir_name_env_stu; do

    if [ $i == $dir_name_env_stu ]; then
        echo "[ENG] Executing ${in_script_file_name_stu} with the scripter."
        echo "[ESP] Ejecutando ${in_script_file_name_stu} las pruebas con el scripter."
    else
        echo "[ENG] Executing ${in_script_file_name_bash} with shell code."
        echo "[ESP] Ejecutando ${in_script_file_name_bash} con el intérprete de comandos."
    fi

    # Remove previous test environment
    if [ -d $i ]; then
        rm -rf $i
    fi
    mkdir -p $i # Create new test environment
    cd $i

    # Create some random files and directories
    cat >foo.txt <<EOF
123432
67890
12345
46789
90a
10a
EOF
    echo "file1" >file1.txt
    echo "file2" >file2.txt
    echo "file3" >file3.txt

    mkdir dir1
    echo "file4" >dir1/file4.txt
    echo "file5" >dir1/file5.txt
    echo "file6" >dir1/file6.txt
    echo "file7" >dir1/file7.txt
    echo -e "el veloz murciélago hindú \ncomía feliz \ncardillo y kiwi" >dir1/file7.txt

    # Copy mygrep to the test environment
    cp ../mygrep .

    #Make sure all files got the right permissions
    chmod -R 755 .

    # if we are in the student environment execute the scripter otherwise make a bash execution
    if [ $i == $dir_name_env_stu ]; then
        # students output
        #echo "../scripter ${PWD}/../InputScripts/$in_script_file_name_stu > ${PWD}/../${output_stu} 2> ${PWD}/../${error_stu}"
        ../scripter ${PWD}/../InputScripts/$in_script_file_name_stu 2>${PWD}/../${error_stu} | sed -E 's/(\[|"|\*)?[0-9]+(\]|"|\*)?//g' >${PWD}/../${output_stu}
    else
        # SO output
        #echo "${PWD}/../InputScripts/$in_script_file_name_bash > ${PWD}/../${output_shell} 2> ${PWD}/../${error_shell}"
        ${PWD}/../InputScripts/$in_script_file_name_bash 2>${PWD}/../${error_shell} | sed -E 's/(\[|"|\*)?[0-9]+(\]|"|\*)?//g' >${PWD}/../${output_shell}
    fi
    cd ..
done

# # Compare outputs and print a percentage of their similarity.
CalculateSimilarity $output_shell $output_stu
similarity=$?
echo " ########### Summary / Resumen ########## "
echo " +++ Similarity for standard output / Similitud para la salida estándar: $similarity%"

# Do the same operation for the error file.
CalculateSimilarity $error_shell $error_stu
similarity=$?
echo " +++ Similarity for standard error / Similitud para la salida estándar de errores: $similarity%"

if [ ${test_level} -gt 1 ]; then # First test does no have redirections.
    # Do the same operations for redirections files.
    # Get a list of all the files in the directory and sort them.
    files1=($(ls ${dir_name_env_shell}/*.out | sort))
    files2=($(ls ${dir_name_env_stu}/*.out | sort))
    for i in "${!files1[@]}"; do
        file1="${files1[$i]}"
        file2="${files2[$i]}"
        # Calculates the similarity of both files.
        CalculateSimilarity ${file1} ${file2}
        similarity=$?
        echo " +++ Similarity for redirection of output to file / Similitud para la redirección de salida al fichero ${file1}: $similarity%"
    done

    CalculateSimilarity $dir_name_env_shell/*.err $dir_name_env_stu/*.err
    similarity=$?
    echo " +++ Similarity for redirection of errors to file / Similitud para la redirección de errores a fichero: $similarity%"
fi
